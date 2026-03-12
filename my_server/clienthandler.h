#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H


#include <QObject>
#include <QTcpSocket>
#include <QString>

class ChatService;

class ClientHandler : public QObject
{
    Q_OBJECT
public:
    explicit ClientHandler(QTcpSocket* socket, QObject *parent = nullptr, ChatService* chatSvc = nullptr);


    QTcpSocket* getSocket() const { return socket; }

    QString parseRequest(const QString& request);
    QString loginUser(const QString& login, const QString& password);
    QString registerUser(const QString& login, const QString& password);
    QString handleSendMessage(const QString& sender, const QString& receiver, const QString& message);
    QString setUserStatus(const QString& user, const QString& status);
    QString getChatHistory(const QString& user1, const QString& user2);
private:
    QTcpSocket* socket;
    ChatService* m_chatService;
};

#endif // CLIENTHANDLER_H


