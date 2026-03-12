#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <QString>
#include <QList>

class ClientHandler;

class ChatService
{
public:
    ChatService();

    QString loginUser(const QString& login, const QString& password);
    QString registerUser(const QString& login, const QString& password);
    void deliverMessage(const QString& sender, const QString& receiver, const QString& message);
    QString setUserStatus(const QString& user, const QString& status);
    QString getChatHistory(const QString& user1, const QString& user2);
    bool saveMessage(const QString& sender,const QString& receiver, const QString& message);

private:

};

#endif
