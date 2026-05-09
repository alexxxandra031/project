#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DatabaseManager;

class DatabaseDestroyer
{
private:
    DatabaseManager* p_instance;
public:
    ~DatabaseDestroyer();
    void initialize(DatabaseManager* p);
};

class DatabaseManager
{
private:
    static DatabaseManager* p_instance;
    static DatabaseDestroyer destroyer;

    QSqlDatabase db;

protected:
    DatabaseManager();

public:
    static DatabaseManager* getInstance();

    bool connect();

    bool createTables();
    bool createMainChatWithAdmin(const QString& adminUsername, const QString& adminPassword);

    enum DbResult {
        DB_OK,
        DB_ERROR,
        DB_USER_EXISTS,
        DB_USER_NOT_FOUND,
        DB_WRONG_PASSWORD
    };

    bool isUserInChat(int userId, int chatId);
    int getUserIdByUsername(const QString& username);
    int getChatCreator(int chatId);
    QList<int> getChatParticipants(int chatId);

    DbResult registerUser(const QString& username, const QString& password, int& outUserId);
    DbResult loginUser(const QString& username, const QString& password, int& outUserId);
    DbResult getUserInfo(int userId, QVariantMap& outUser);
    DbResult updateUserStatus(int userId, const QString& status);

    DbResult createChat(int creatorId, const QString& type, int& outChatId);
    DbResult getChatInfo(int chatId, QVariantMap& outChat);
    DbResult getChatParticipantsUsernames(int chatId, QList<QVariantMap>& outUsers);
    DbResult updateChatName(int chatId, const QString& newName);

    DbResult addUserToChat(int chatId, int userId, const QString& role);
    DbResult removeUserFromChat(int chatId, int userId);

    DbResult saveMessage(int userId, int chatId, const QString& message);
    DbResult getChatHistory(int chatId, QList<QVariantMap>& outMessages);

    DbResult findUsers(const QString& search, QList<QString>& outUsers);

    int getUserCount();
    int getMessageCount();

    friend class DatabaseDestroyer;
};

#endif