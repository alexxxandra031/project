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

    bool checkAuthorized();
    bool checkNotAuthorized();

    void parseRequest(const QString& request);

    // --- обработчики команд ---
    void handleRegisterUser(const QString& login, const QString& password);
    void handleLoginUser(const QString& login, const QString& password);
    void handleLogout();

    void handleGetUserInfo();

    void handleCreateChat();
    void handleGetChatInfo(const QString& chatIdStr);
    void handleChangeChatName(const QString& chatIdStr, const QString& newName);

    void handleAddUser(const QString& chatIdStr, const QString& username);
    void handleRemoveUser(const QString& chatIdStr, const QString& username);
    void handleLeaveChat(const QString& chatIdStr);

    void handleSendMessage(const QString& chatIdStr, const QString& message);
    void handleGetChatHistory(const QString& chatIdStr);

    void handleFindUsers(const QString& search);

    void handleGetStats();

    static void addOnlineClient(int userId, QTcpSocket* socket) {
        m_onlineUserSockets[userId] = socket;
        m_onlineSocketUsers[socket] = userId;
    }

    static void removeOnlineClient(QTcpSocket* socket) {
        if(!m_onlineSocketUsers.contains(socket))
            return;

        int userId = m_onlineSocketUsers.value(socket, -1);
        if(userId != -1) {
            m_onlineUserSockets.remove(userId);
            m_onlineSocketUsers.remove(socket);
        }
    }

private:
    void sendToClient(QTcpSocket* socket, const QString& message);

    QTcpSocket* socket;
    ChatService* m_chatService;

    int m_userId = -1;
    QString m_username;

    static QMap<int, QTcpSocket*> m_onlineUserSockets;  // userId -> socket
    static QMap<QTcpSocket*, int> m_onlineSocketUsers;  // socket -> userId

};

#endif // CLIENTHANDLER_H


