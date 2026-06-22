#include "databasemanager.h"

DatabaseManager* DatabaseManager::p_instance = nullptr;
DatabaseDestroyer DatabaseManager::destroyer;

DatabaseDestroyer::~DatabaseDestroyer()
{
    delete p_instance;
}

void DatabaseDestroyer::initialize(DatabaseManager* p)
{
    p_instance = p;
}

DatabaseManager::DatabaseManager()
{
}

DatabaseManager* DatabaseManager::getInstance()
{
    if (!p_instance)
    {
        p_instance = new DatabaseManager();
        destroyer.initialize(p_instance);
    }

    return p_instance;
}



bool DatabaseManager::connect()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("chat.db");

    if(!db.open())
    {
        qDebug() << db.lastError().text();
        return false;
    }

    qDebug() << "Database connected";

    QSqlQuery query;
    query.exec("PRAGMA foreign_keys = ON");

    createTables();

    createMainChatWithAdmin("ADMIN", "123");

    return true;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query;

    bool success = true;

    success &= query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE,"
        "password TEXT,"
        "role TEXT,"
        "status TEXT,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")"
        );

    success &= query.exec(
        "CREATE TABLE IF NOT EXISTS chats ("
        "chat_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "chat_name TEXT,"
        "type TEXT,"
        "created_at TEXT,"
        "creator_id INTEGER,"
        "FOREIGN KEY(creator_id) REFERENCES users(user_id)"
        ")"
        );

    success &= query.exec(
        "CREATE TABLE IF NOT EXISTS chat_participants ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "chat_id INTEGER,"
        "user_id INTEGER,"
        "role TEXT,"
        "FOREIGN KEY(chat_id) REFERENCES chats(chat_id) ON DELETE CASCADE,"
        "FOREIGN KEY(user_id) REFERENCES users(user_id)"
        ")"
        );

    success &= query.exec(
        "CREATE TABLE IF NOT EXISTS messages ("
        "message_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "chat_id INTEGER,"
        "sender_id INTEGER,"
        "content TEXT,"
        "created_at TEXT,"
        "message_status TEXT,"
        "FOREIGN KEY(chat_id) REFERENCES chats(chat_id),"
        "FOREIGN KEY(sender_id) REFERENCES users(user_id)"
        ")"
        );

    if(!success)
        qDebug() << "Error creating tables:" << query.lastError().text();
    else
        qDebug() << "Tables ready";

    return success;
}

bool DatabaseManager::createMainChatWithAdmin(const QString& adminUsername, const QString& adminPassword) {
    QSqlQuery query;
-
    query.prepare("INSERT OR IGNORE INTO users(username, password, role, status) "
                  "VALUES (:username, :password, 'admin', 'offline')");
    query.bindValue(":username", adminUsername);
    query.bindValue(":password", adminPassword);

    if(!query.exec()) {
        qDebug() << "Ошибка создания админа:" << query.lastError();
        return false;
    }


    int adminId = -1;
    query.prepare("SELECT user_id FROM users WHERE username = :username");
    query.bindValue(":username", adminUsername);
    if(query.exec() && query.next()) {
        adminId = query.value(0).toInt();
    } else {
        qDebug() << "Не удалось получить ID админа";
        return false;
    }


    query.prepare("INSERT OR IGNORE INTO chats(chat_id, chat_name, type, creator_id) "
                  "VALUES (1, 'Global Chat', 'global', :creator_id)");
    query.bindValue(":creator_id", adminId);
    if(!query.exec()) {
        qDebug() << "Ошибка создания глобального чата:" << query.lastError();
        return false;
    }

    query.prepare("INSERT OR IGNORE INTO chat_participants(chat_id, user_id, role) "
                  "VALUES (1, :user_id, 'creator')");
    query.bindValue(":user_id", adminId);
    if(!query.exec()) {
        qDebug() << "Ошибка добавления админа в чат:" << query.lastError();
        return false;
    }

    return true;
}







bool DatabaseManager::isUserInChat(int userId, int chatId)
{
    QSqlQuery query;

    query.prepare(
        "SELECT 1 FROM chat_participants "
        "WHERE user_id = :user_id AND chat_id = :chat_id"
        );

    query.bindValue(":user_id", userId);
    query.bindValue(":chat_id", chatId);

    if(!query.exec()) return false;

    return query.next();
}

int DatabaseManager::getUserIdByUsername(const QString& username)
{
    QSqlQuery query;
    query.prepare("SELECT user_id FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if(!query.exec()) {
        qDebug() << "[DB ERROR] getUserIdByUsername:" << query.lastError();
        return -1;
    }

    if(query.next())
        return query.value(0).toInt();

    return -1; // пользователь не найден
}




int DatabaseManager::getChatCreator(int chatId)
{
    QSqlQuery query;
    query.prepare("SELECT creator_id FROM chats WHERE chat_id = :chatId");
    query.bindValue(":chatId", chatId);

    if(!query.exec() || !query.next())
        return -1;

    return query.value(0).toInt();
}

QList<int> DatabaseManager::getChatParticipants(int chatId)
{
    QList<int> participants;

    // Проверка, существует ли чат
    QSqlQuery query;
    query.prepare("SELECT user_id FROM chat_participants WHERE chat_id = :chatId");
    query.bindValue(":chatId", chatId);

    if(!query.exec()) {
        qDebug() << "getChatParticipants: ошибка SQL" << query.lastError().text();
        return participants;
    }

    while(query.next()) {
        participants.append(query.value("user_id").toInt());
    }

    return participants;
}





DatabaseManager::DbResult DatabaseManager::registerUser(const QString& username, const QString& password, int& outUserId)
{
    QSqlQuery query;

    // проверка существования
    query.prepare("SELECT user_id FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if(!query.exec())
        return DB_ERROR;

    if(query.next())
        return DB_USER_EXISTS;


    query.prepare("INSERT INTO users (username, password, status, role) VALUES (:username, :password, 'offline', 'user')");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if(!query.exec())
        return DB_ERROR;


    outUserId = query.lastInsertId().toInt();

    return DB_OK;
}

DatabaseManager::DbResult DatabaseManager::loginUser(const QString& username, const QString& password, int& outUserId)
{
    QSqlQuery query;

    query.prepare("SELECT user_id FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if(!query.exec())
    {
        qDebug() << "Ошибка запроса login:" << query.lastError().text();
        return DB_ERROR;
    }

    if(query.next())
    {
        outUserId = query.value(0).toInt();
        updateUserStatus(outUserId, "online");
        return DB_OK;
    }

    query.prepare("SELECT user_id FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if(query.exec() && query.next())
    {
        return DB_WRONG_PASSWORD;
    }

    return DB_USER_NOT_FOUND;
}



DatabaseManager::DbResult DatabaseManager::getUserInfo(int userId, QVariantMap& outUser)
{
    outUser.clear();

    QSqlQuery query;
    query.prepare("SELECT user_id, username, role, status, created_at FROM users WHERE user_id = :id");
    query.bindValue(":id", userId);

    if(!query.exec()) {
        qDebug() << "Ошибка запроса пользователя:" << query.lastError();
        return DB_ERROR;
    }

    if(!query.next())
        return DB_USER_NOT_FOUND;

    outUser["user_id"] = query.value("user_id");
    outUser["username"] = query.value("username");
    outUser["role"] = query.value("role");
    outUser["status"] = query.value("status");
    outUser["created_at"] = query.value("created_at");


    QSqlQuery chatQuery;
    chatQuery.prepare("SELECT chat_id FROM chat_participants WHERE user_id = :id");
    chatQuery.bindValue(":id", userId);

    QList<int> chatIds;
    if(chatQuery.exec()) {
        while(chatQuery.next()) {
            chatIds.append(chatQuery.value("chat_id").toInt());
        }
    }

    outUser["chats"] = QVariant::fromValue(chatIds);

    return DB_OK;
}



DatabaseManager::DbResult DatabaseManager::updateUserStatus(int userId, const QString& status)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET status = :status WHERE user_id = :id");
    query.bindValue(":status", status);
    query.bindValue(":id", userId);

    if(!query.exec())
    {
        qDebug() << "Ошибка обновления статуса:" << query.lastError();
        return DB_ERROR;
    }

    return DB_OK;
}

DatabaseManager::DbResult DatabaseManager::createChat(int creatorId, const QString& type, int& outChatId)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO chats (chat_name, type, created_at, creator_id) "
        "VALUES ('Новый чат', :type, datetime('now'), :creator_id)"
        );
    query.bindValue(":type", type);
    query.bindValue(":creator_id", creatorId);

    if(!query.exec())
    {
        qDebug() << "Ошибка создания чата:" << query.lastError();
        return DB_ERROR;
    }

    outChatId = query.lastInsertId().toInt();
    return DB_OK;
}


DatabaseManager::DbResult DatabaseManager::getChatInfo(int chatId, QVariantMap& outChat)
{
    outChat.clear();

    QSqlQuery query;
    query.prepare(
        "SELECT chat_name, type "
        "FROM chats "
        "WHERE chat_id = :chatId"
        );
    query.bindValue(":chatId", chatId);

    if(!query.exec()) {
        qDebug() << "getChatInfo error:" << query.lastError();
        return DB_ERROR;
    }

    if(!query.next())
        return DB_ERROR;

    outChat["chat_name"] = query.value(0).toString();
    outChat["type"] = query.value(1).toString();

    return DB_OK;
}


DatabaseManager::DbResult DatabaseManager::getChatParticipantsUsernames(
    int chatId,
    QList<QVariantMap>& outUsers)
{
    outUsers.clear();

    QSqlQuery query;
    query.prepare(
        "SELECT u.username, u.status "
        "FROM chat_participants cp "
        "JOIN users u ON cp.user_id = u.user_id "
        "WHERE cp.chat_id = :chatId AND u.role != 'admin'"
        );
    query.bindValue(":chatId", chatId);

    if(!query.exec()) {
        qDebug() << "getChatParticipants error:" << query.lastError();
        return DB_ERROR;
    }

    while(query.next()) {
        QVariantMap user;
        user["username"] = query.value(0).toString();
        user["status"] = query.value(1).toString();
        outUsers.append(user);
    }

    return DB_OK;
}


DatabaseManager::DbResult DatabaseManager::updateChatName(int chatId, const QString& newName)
{
    QSqlQuery query;
    query.prepare("UPDATE chats SET chat_name = :name WHERE chat_id = :chatId");
    query.bindValue(":name", newName);
    query.bindValue(":chatId", chatId);

    if(!query.exec()) {
        qDebug() << "Ошибка обновления имени чата:" << query.lastError();
        return DB_ERROR;
    }

    return DB_OK;
}


DatabaseManager::DbResult DatabaseManager::addUserToChat(int chatId, int userId, const QString& role)
{

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT 1 FROM chat_participants WHERE chat_id = :chat_id AND user_id = :user_id");
    checkQuery.bindValue(":chat_id", chatId);
    checkQuery.bindValue(":user_id", userId);

    if(!checkQuery.exec())
    {
        qDebug() << "Ошибка проверки участника в чате:" << checkQuery.lastError();
        return DB_ERROR;
    }

    if(checkQuery.next())
    {
        // Пользователь уже есть в чате
        return DB_USER_EXISTS;
    }


    QSqlQuery insertQuery;
    insertQuery.prepare(
        "INSERT INTO chat_participants (chat_id, user_id, role) "
        "VALUES (:chat_id, :user_id, :role)"
        );
    insertQuery.bindValue(":chat_id", chatId);
    insertQuery.bindValue(":user_id", userId);
    insertQuery.bindValue(":role", role);

    if(!insertQuery.exec())
    {
        qDebug() << "Ошибка добавления участника:" << insertQuery.lastError();
        return DB_ERROR;
    }

    return DB_OK;
}
\

    DatabaseManager::DbResult DatabaseManager::removeUserFromChat(int chatId, int userId)
{
    QSqlQuery query;
    query.prepare(
        "DELETE FROM chat_participants "
        "WHERE chat_id = :chat_id AND user_id = :user_id"
        );
    query.bindValue(":chat_id", chatId);
    query.bindValue(":user_id", userId);

    if(!query.exec()) {
        qDebug() << "Ошибка удаления участника:" << query.lastError();
        return DB_ERROR;
    }

    return DB_OK;
}



DatabaseManager::DbResult DatabaseManager::saveMessage(int userId, int chatId, const QString& message)
{
    QSqlQuery query;
    query.prepare("INSERT INTO messages (chat_id, sender_id, content, created_at, message_status) "
                  "VALUES (:chat_id, :sender_id, :content, datetime('now'), 'sent')");
    query.bindValue(":chat_id", chatId);
    query.bindValue(":sender_id", userId);
    query.bindValue(":content", message);

    if(!query.exec()) {
        qDebug() << "Ошибка сохранения сообщения:" << query.lastError();
        return DB_ERROR;
    }

    return DB_OK;
}

DatabaseManager::DbResult DatabaseManager::getChatHistory(int chatId, QList<QVariantMap>& outMessages)
{
    outMessages.clear();

    QSqlQuery query;
    query.prepare(
        "SELECT m.message_id, u.username, m.content, m.created_at "
        "FROM messages m "
        "JOIN users u ON m.sender_id = u.user_id "
        "WHERE m.chat_id = :chat_id "
        "ORDER BY m.created_at ASC"
        );
    query.bindValue(":chat_id", chatId);

    if(!query.exec())
    {
        qDebug() << "Ошибка истории чата:" << query.lastError();
        return DB_ERROR; // вернули ошибку загрузки
    }

    while(query.next())
    {
        QVariantMap msg;
        msg["id"] = query.value(0).toInt();
        msg["user"] = query.value(1).toString();
        msg["message"] = query.value(2).toString();
        msg["time"] = query.value(3).toString();
        outMessages.append(msg);
    }

    return DB_OK;
}






DatabaseManager::DbResult DatabaseManager::findUsers(
    const QString& search,
    QList<QString>& outUsers)
{
    outUsers.clear();

    QSqlQuery query;
    query.prepare(
        "SELECT username FROM users "
        "WHERE username LIKE :search AND role != 'admin'"
        );
    query.bindValue(":search", "%" + search + "%");

    if(!query.exec()) {
        qDebug() << "findUsers error:" << query.lastError();
        return DB_ERROR;
    }

    while(query.next())
        outUsers.append(query.value(0).toString());

    if(outUsers.isEmpty())
        return DB_USER_NOT_FOUND;

    return DB_OK;
}



int DatabaseManager::getUserCount() {
    QSqlQuery query;
    query.exec("SELECT COUNT(*) FROM users");
    if(query.next())
        return query.value(0).toInt();
    return 0;
}

int DatabaseManager::getMessageCount() {
    QSqlQuery query;
    query.exec("SELECT COUNT(*) FROM messages");
    if(query.next())
        return query.value(0).toInt();
    return 0;
}

