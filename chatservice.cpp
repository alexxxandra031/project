#include "chatservice.h"
#include "clienthandler.h"
#include "databasemanager.h"
#include <QDateTime>

ChatService::ChatService()
{
    qDebug() << "ChatService создан";
}

//метод для успешного ответа клиенту
QString ChatService::okResponse(const QString& type, const QString& data) const
{
    if(data.isEmpty())
        return "OK|" + type;
    return "OK|" + type + "|" + data;
}

//метод для ответа-ошибки клиенту
QString ChatService::errorResponse(const QString& type) const
{
    return "ERROR|" + type;
}

// Регистрация пользователя
QString ChatService::registerUser(const QString& username, const QString& password, int& outUserId)
{

    DatabaseManager::DbResult res = DatabaseManager::getInstance()->registerUser(username, password, outUserId);

    switch(res) {
    case DatabaseManager::DB_OK:
        qDebug() << "[REGISTER OK]";
        DatabaseManager::getInstance()->addUserToChat(1, outUserId, "member");
        return okResponse("REGISTER");
    case DatabaseManager::DB_USER_EXISTS:
        qDebug() << "[ERROR] USER EXISTS";
        return errorResponse("USER_EXISTS");
    default:
        qDebug() << "[ERROR] REGISTER ERROR";
        return errorResponse("REGISTER_ERROR");
    }
}
//авторизация
QString ChatService::loginUser(const QString& username, const QString& password, int& outUserId)
{

    DatabaseManager::DbResult res = DatabaseManager::getInstance()->loginUser(username, password, outUserId);

    switch(res) {
    case DatabaseManager::DB_OK:
        qDebug() << "[OK] LOGIN" << outUserId;
        return okResponse("LOGIN");

    case DatabaseManager::DB_WRONG_PASSWORD:
        qDebug() << "[ERROR] WRONG PASSWORD";
        return errorResponse("LOGIN_WRONG_PASSWORD");

    case DatabaseManager::DB_USER_NOT_FOUND:
        qDebug() << "[ERROR] USER NOT FOUND";
        return errorResponse("LOGIN_USER_NOT_FOUND");

    default:
        qDebug() << "[ERROR] LOGIN ERROR";
        return errorResponse("LOGIN_ERROR");
    }
}



// Получение информации о пользователе
QString ChatService::getUserInfo(int userId)
{

    QVariantMap user;
    DatabaseManager::DbResult res = DatabaseManager::getInstance()->getUserInfo(userId, user);

    if(res != DatabaseManager::DB_OK) {
        qDebug() << "[ERROR] USER NOT FOUND";
        return errorResponse("USER_NOT_FOUND");
    }
    QJsonObject obj;
    obj["userId"] = user["user_id"].toInt();
    obj["username"] = user["username"].toString();
    obj["role"] = user["role"].toString();
    obj["status"] = user["status"].toString();
    obj["createdAt"] = user["created_at"].toString();

    QJsonArray chatArray;
    for(const auto& chatId : user["chats"].toList())
        chatArray.append(chatId.toInt());
    obj["chats"] = chatArray;

    QJsonDocument doc(obj);
    qDebug() << "[USER INFO OK]";
    return okResponse("USER_INFO", doc.toJson(QJsonDocument::Compact));
}


// Создание чата
QString ChatService::createChat(int creatorId)
{
    int chatId = -1;
    DatabaseManager::DbResult res = DatabaseManager::getInstance()->createChat(creatorId, "group", chatId);

    if(res != DatabaseManager::DB_OK) {
        qDebug() << "[ERROR] CREATE_CHAT_FAILED";
        return errorResponse("CREATE_CHAT_FAILED");
    }
    DatabaseManager::getInstance()->addUserToChat(chatId, creatorId, "creator");
    qDebug() << "[CREATE_CHAT OK]" << chatId;
    return okResponse("CREATE_CHAT", QString::number(chatId));
}

QString ChatService::getChatInfo(int userId, int chatId)
{
    // проверка доступа
    if(!DatabaseManager::getInstance()->isUserInChat(userId, chatId))
        return errorResponse("NOT_IN_CHAT");

    // данные чата
    QVariantMap chat;
    if(DatabaseManager::getInstance()->getChatInfo(chatId, chat) != DatabaseManager::DB_OK)
        return errorResponse("CHAT_NOT_FOUND");

    // участники
    QList<QVariantMap> users;
    if(DatabaseManager::getInstance()->getChatParticipantsUsernames(chatId, users)
        != DatabaseManager::DB_OK)
        return errorResponse("CHAT_PARTICIPANTS_ERROR");


    QJsonArray participantsArray;
    for(const auto& user : users) {
        QJsonObject obj;
        obj["username"] = user["username"].toString();
        obj["status"] = user["status"].toString();
        participantsArray.append(obj);
    }

    // итоговый JSON
    QJsonObject chatObj;
    chatObj["chatName"] = chat["chat_name"].toString();
    chatObj["type"] = chat["type"].toString();
    chatObj["participants"] = participantsArray;

    QJsonDocument doc(chatObj);
    return okResponse("CHAT_INFO", doc.toJson(QJsonDocument::Compact));
}

QString ChatService::changeChatName(int userId, int chatId, const QString& newName)
{
    if(chatId == 1)  // если ID глобального чата = 1
        return errorResponse("CANNOT_RENAME_GLOBAL_CHAT");

    // проверка, что пользователь в чате
    if(!DatabaseManager::getInstance()->isUserInChat(userId, chatId))
        return errorResponse("NOT_IN_CHAT");




    if(DatabaseManager::getInstance()->updateChatName(chatId, newName) != DatabaseManager::DB_OK)
        return errorResponse("CHANGE_NAME_FAILED");

    return okResponse("CHANGE_CHAT_NAME");
}

// Добавление пользователя в чат

QString ChatService::addUserToChat(int chatId, const QString& username, int requesterId)
{

    if(!DatabaseManager::getInstance()->isUserInChat(requesterId, chatId)) {
        qDebug() << "[ERROR] NOT_IN_CHAT";
        return errorResponse("NOT_IN_CHAT");
    }

    int userId = DatabaseManager::getInstance()->getUserIdByUsername(username);

    if(userId == -1) {
        qDebug() << "[ERROR] USER NOT FOUND:" << username;
        return errorResponse("USER_NOT_FOUND");
    }

    QVariantMap user;
    DatabaseManager::getInstance()->getUserInfo(userId, user);

    if(user["role"].toString() == "admin") {
        return errorResponse("CANNOT_ADD_ADMIN");
    }

    DatabaseManager::DbResult res =
        DatabaseManager::getInstance()->addUserToChat(chatId, userId, "member");

    switch(res) {
    case DatabaseManager::DB_OK:
        qDebug() << "[ADD_USER OK]";
        return okResponse("ADD_USER");

    case DatabaseManager::DB_USER_EXISTS:
        qDebug() << "[ERROR] USER_ALREADY_IN_CHAT:" << username;
        return errorResponse("USER_ALREADY_IN_CHAT");

    default:
        qDebug() << "[ERROR] ADD_USER_FAILED";
        return errorResponse("ADD_USER_FAILED");
    }
}

QString ChatService::removeUserFromChat(int chatId, const QString& username, int requesterId)
{
    if(!DatabaseManager::getInstance()->isUserInChat(requesterId, chatId))
        return errorResponse("NOT_IN_CHAT");

    int creatorId = DatabaseManager::getInstance()->getChatCreator(chatId);

    // только создатель может удалять
    if(requesterId != creatorId)
        return errorResponse("NOT_CREATOR");

    int userId = DatabaseManager::getInstance()->getUserIdByUsername(username);

    if(userId == -1)
        return errorResponse("USER_NOT_FOUND");

    // нельзя удалить создателя
    if(userId == creatorId)
        return errorResponse("CANNOT_REMOVE_CREATOR");

    DatabaseManager::DbResult res =
        DatabaseManager::getInstance()->removeUserFromChat(chatId, userId);

    if(res != DatabaseManager::DB_OK)
        return errorResponse("REMOVE_FAILED");

    return okResponse("REMOVE_USER");
}

QString ChatService::leaveChat(int userId, int chatId)
{
    if(!DatabaseManager::getInstance()->isUserInChat(userId, chatId))
        return errorResponse("NOT_IN_CHAT");

    int creatorId = DatabaseManager::getInstance()->getChatCreator(chatId);


    if(userId == creatorId)
        return errorResponse("CREATOR_CANNOT_LEAVE");

    DatabaseManager::DbResult res =
        DatabaseManager::getInstance()->removeUserFromChat(chatId, userId);

    if(res != DatabaseManager::DB_OK)
        return errorResponse("LEAVE_FAILED");

    return okResponse("LEAVE_CHAT");
}


// 1. Сохранение сообщение в БД
QString ChatService::saveMessage(int senderId, int chatId, const QString& message)
{
    if(message.trimmed().isEmpty()) {
        qDebug() << "[ERROR] EMPTY_MESSAGE from user" << senderId;
        return errorResponse("EMPTY_MESSAGE");
    }

    if(!DatabaseManager::getInstance()->isUserInChat(senderId, chatId)) {
        qDebug() << "[SECURITY] User" << senderId << "not in chat" << chatId;
        return errorResponse("NOT_IN_CHAT");
    }

    DatabaseManager::DbResult res = DatabaseManager::getInstance()->saveMessage(senderId, chatId, message);

    if(res != DatabaseManager::DB_OK)
        return errorResponse("SAVE_FAILED");

    return okResponse("SAVE");
}

// 2. Формирование JSON для клиента
QString ChatService::formatNewMessage(int chatId, const QString& username, const QString& message)
{
    QJsonObject obj;
    obj["chatId"] = chatId;
    obj["username"] = username;
    obj["message"] = message;
	obj["time"] =
	QDateTime::currentDateTime().toString("hh:mm");

    QJsonDocument doc(obj);
    return "NEW_MESSAGE|" + doc.toJson(QJsonDocument::Compact);
}


QString ChatService::getChatHistory(int userId, int chatId)
{
    if(!DatabaseManager::getInstance()->isUserInChat(userId, chatId))
        return errorResponse("NOT_IN_CHAT");

    QList<QVariantMap> messages;
    DatabaseManager::DbResult res = DatabaseManager::getInstance()->getChatHistory(chatId, messages);

    if(res == DatabaseManager::DB_ERROR)
        return errorResponse("CHAT_LOAD_ERROR");

    QJsonArray jsonArray;
    for(const auto& msg : messages) {
        QJsonObject obj;
        obj["id"] = msg["id"].toInt();
        obj["user"] = msg["user"].toString();
        obj["message"] = msg["message"].toString();
        obj["time"] = msg["time"].toString();
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    return okResponse("HISTORY", doc.toJson(QJsonDocument::Compact));
}


QString ChatService::findUsers(const QString& search)
{


    QList<QString> users;
    DatabaseManager::DbResult res = DatabaseManager::getInstance()->findUsers(search, users);

    switch(res) {
    case DatabaseManager::DB_OK:
        return okResponse("FIND_USERS", users.join(","));
    case DatabaseManager::DB_USER_NOT_FOUND:
        return errorResponse("USER_NOT_FOUND");
    case DatabaseManager::DB_ERROR:
    default:
        return errorResponse("DB_ERROR");
    }
}



QString ChatService::getStats(int userId)
{

    QVariantMap user;
    if(DatabaseManager::getInstance()->getUserInfo(userId, user) != DatabaseManager::DB_OK)
        return errorResponse("USER_NOT_FOUND");

    if(user["role"].toString() != "admin")
        return errorResponse("NOT_ADMIN");

    int userCount = DatabaseManager::getInstance()->getUserCount();
    int messageCount = DatabaseManager::getInstance()->getMessageCount();


    QJsonObject obj;
    obj["users"] = userCount;
    obj["messages"] = messageCount;

    QJsonDocument doc(obj);
    return okResponse("STAT", doc.toJson(QJsonDocument::Compact));
}

