#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <QString>
#include <QVariantMap>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class DatabaseManager;

class ChatService
{
public:
    ChatService();

    QString okResponse(const QString& type, const QString& data = "") const;
    QString errorResponse(const QString& type) const;

    QString registerUser(const QString& username, const QString& password, int& outUserId);
    QString loginUser(const QString& username, const QString& password, int& outUserId);

    QString getUserInfo(int userId);

    // --- чаты ---
    QString createChat(int creatorId);
    QString getChatInfo(int userId, int chatId);
    QString changeChatName(int userId, int chatId, const QString& newName);

    QString addUserToChat(int chatId, const QString& username, int requesterId);
    QString removeUserFromChat(int chatId, const QString& username, int requesterId);
    QString leaveChat(int userId, int chatId);

    // --- сообщения ---
    QString saveMessage(int senderId, int chatId, const QString& message);
    QString formatNewMessage(int chatId, const QString& username, const QString& message);
    QString getChatHistory(int userId, int chatId);

    QString findUsers(const QString& search);

    QString getStats(int userId);

private:

};

#endif