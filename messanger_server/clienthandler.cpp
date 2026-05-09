#include "clienthandler.h"
#include "chatservice.h"
#include "databasemanager.h"

ClientHandler::ClientHandler(QTcpSocket* sock, QObject* parent, ChatService* chatSvc)
    : QObject(parent), socket(sock), m_chatService(chatSvc)
{
    qDebug() << "[INFO] ClientHandler created for socket:" << sock;
}

QMap<int, QTcpSocket*> ClientHandler::m_onlineUserSockets;
QMap<QTcpSocket*, int> ClientHandler::m_onlineSocketUsers;

void ClientHandler::parseRequest(const QString& request)
{
    QStringList parts = request.split("|", Qt::KeepEmptyParts);
    QString command = parts.value(0).trimmed();
    qDebug() << "[REQUEST]" << command << "from userId:" << m_userId << "full:" << request;

    if(command == "REGISTER") {
        if(!checkNotAuthorized()) return;
        if(parts.size() != 3) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleRegisterUser(parts[1], parts[2]);
    }
    else if(command == "LOGIN") {
        if(!checkNotAuthorized()) return;
        if(parts.size() != 3) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleLoginUser(parts[1], parts[2]);
    }
    else if(command == "LOGOUT") {
        if(parts.size() != 1) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleLogout();
    }
    else if(command == "USER_INFO") {
        if(parts.size() != 1) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleGetUserInfo();
    }
    else if(command == "CREATE_CHAT") {
        if(parts.size() != 1) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleCreateChat();
    }
    else if(command == "CHAT_INFO") {
        if(parts.size() != 2) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleGetChatInfo(parts[1]);
    }
    else if(command == "CHANGE_CHAT_NAME") {
        if(parts.size() != 3) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleChangeChatName(parts[1], parts[2]);
    }
    else if(command == "ADD_USER") {
        if(parts.size() != 3) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleAddUser(parts[1], parts[2]);
    }
    else if(command == "REMOVE_USER") {
        if(parts.size() != 3) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleRemoveUser(parts[1], parts[2]);
    }
    else if(command == "LEAVE_CHAT") {
        if(parts.size() != 2) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleLeaveChat(parts[1]);
    }
    else if(command == "SEND") {
        if(parts.size() != 3) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleSendMessage(parts[1], parts[2]);
    }
    else if(command == "HISTORY") {
        if(parts.size() != 2) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleGetChatHistory(parts[1]);
    }
    else if(command == "FIND_USERS") {
        if(parts.size() != 2) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleFindUsers(parts[1]);
    }
    else if(command == "STAT") {
        if(parts.size() != 1) {
            qDebug() << "[ERROR] INVALID_FORMAT for command" << command << "payload:" << request;
            sendToClient(socket, "ERROR|INVALID_FORMAT");
            return;
        }
        handleGetStats();
    }
    else {
        qDebug() << "[ERROR] UNKNOWN_COMMAND:" << request;
        sendToClient(socket, "ERROR|UNKNOWN_COMMAND");
    }
}

void ClientHandler::sendToClient(QTcpSocket* socket, const QString& message)
{
    if(socket && socket->isOpen()) {
        socket->write((message).toUtf8());
        socket->flush();
    }
}

bool ClientHandler::checkAuthorized() {
    if (m_userId == -1) {
        sendToClient(socket, "ERROR|NOT_AUTHORIZED");
        qDebug() << "[SECURITY] Unauthorized access attempt";
        return false;
    }
    return true;
}

bool ClientHandler::checkNotAuthorized() {
    if (m_userId != -1) {
        sendToClient(socket, "ERROR|ALREADY_AUTHORIZED");
        qDebug() << "[SECURITY] Already logged in user tried unauthorized action:" << m_userId;
        return false;
    }
    return true;
}

void ClientHandler::handleRegisterUser(const QString& login, const QString& password) {
    qDebug() << "[ACTION] REGISTER attempt userId:" << m_userId << "username:" << login;
    int userId = -1;
    QString response = m_chatService->registerUser(login, password, userId);
    sendToClient(socket, response);
}

void ClientHandler::handleLoginUser(const QString& login, const QString& password) {
    qDebug() << "[ACTION] LOGIN attempt userId:" << m_userId << "username:" << login;
    int userId = -1;
    QString response = m_chatService->loginUser(login, password, userId);
    sendToClient(socket, response);
    if(response.startsWith("OK|LOGIN")) {
        m_userId = userId;
        m_username = login;
        addOnlineClient(userId, socket);
        DatabaseManager::getInstance()->updateUserStatus(userId, "online");
    }
}

void ClientHandler::handleLogout() {
    qDebug() << "[ACTION] LOGOUT userId:" << m_userId;
    if(!checkAuthorized()) return;
    removeOnlineClient(socket);
    if(m_userId != -1)
        DatabaseManager::getInstance()->updateUserStatus(m_userId, "offline");
    m_userId = -1;
    sendToClient(socket, m_chatService->okResponse("LOGOUT"));
}

void ClientHandler::handleGetUserInfo() {
    qDebug() << "[ACTION] USER_INFO request userId:" << m_userId;
    if(!checkAuthorized()) return;
    QString response = m_chatService->getUserInfo(m_userId);
    sendToClient(socket, response);
}

void ClientHandler::handleCreateChat() {
    qDebug() << "[ACTION] CREATE_CHAT userId:" << m_userId;
    if(!checkAuthorized()) return;
    QString response = m_chatService->createChat(m_userId);
    sendToClient(socket, response);
}

void ClientHandler::handleGetChatInfo(const QString& chatIdStr) {
    qDebug() << "[ACTION] CHAT_INFO userId:" << m_userId << "chatId:" << chatIdStr;
    if(!checkAuthorized()) return;
    bool ok;
    int chatId = chatIdStr.toInt(&ok);
    if(!ok) {
        sendToClient(socket, "ERROR|INVALID_CHAT_ID");
        return;
    }
    QString response = m_chatService->getChatInfo(m_userId, chatId);
    sendToClient(socket, response);
}

void ClientHandler::handleChangeChatName(const QString& chatIdStr, const QString& newName) {
    qDebug() << "[ACTION] CHANGE_CHAT_NAME userId:" << m_userId << "chatId:" << chatIdStr << "newName:" << newName;
    if(!checkAuthorized()) return;
    bool ok;
    int chatId = chatIdStr.toInt(&ok);
    QString response = m_chatService->changeChatName(m_userId, chatId, newName);
    sendToClient(socket, response);
}

void ClientHandler::handleAddUser(const QString& chatIdStr, const QString& username) {
    qDebug() << "[ACTION] ADD_USER userId:" << m_userId << "chatId:" << chatIdStr << "targetUser:" << username;
    if(!checkAuthorized()) return;
    bool ok;
    int chatId = chatIdStr.toInt(&ok);
    if(!ok) {
        sendToClient(socket, "ERROR|INVALID_CHAT_ID");
        return;
    }
    QString response = m_chatService->addUserToChat(chatId, username, m_userId);
    sendToClient(socket, response);
}

void ClientHandler::handleRemoveUser(const QString& chatIdStr, const QString& username) {
    qDebug() << "[ACTION] REMOVE_USER userId:" << m_userId << "chatId:" << chatIdStr << "targetUser:" << username;
    if(!checkAuthorized()) return;
    bool ok;
    int chatId = chatIdStr.toInt(&ok);
    QString response = m_chatService->removeUserFromChat(chatId, username, m_userId);
    sendToClient(socket, response);
}

void ClientHandler::handleLeaveChat(const QString& chatIdStr) {
    qDebug() << "[ACTION] LEAVE_CHAT userId:" << m_userId << "chatId:" << chatIdStr;
    if(!checkAuthorized()) return;
    bool ok;
    int chatId = chatIdStr.toInt(&ok);
    QString response = m_chatService->leaveChat(m_userId, chatId);
    sendToClient(socket, response);
}

void ClientHandler::handleSendMessage(const QString& chatIdStr, const QString& message) {
    qDebug() << "[ACTION] SEND_MESSAGE userId:" << m_userId << "chatId:" << chatIdStr << "messageSnippet:" << message.left(20);
    if(!checkAuthorized()) return;
    bool ok;
    int chatId = chatIdStr.toInt(&ok);
    if(!ok) {
        sendToClient(socket, "ERROR|INVALID_CHAT_ID");
        return;
    }
    QString saveResult = m_chatService->saveMessage(m_userId, chatId, message);
    if(!saveResult.startsWith("OK")) {
        sendToClient(socket, saveResult);
        return;
    }
    QString jsonMessage = m_chatService->formatNewMessage(chatId, m_username, message);
    QList<int> participants = DatabaseManager::getInstance()->getChatParticipants(chatId);
    for(int userId : participants) {
        if(m_onlineUserSockets.contains(userId)) {
            sendToClient(m_onlineUserSockets[userId], jsonMessage);
        }
    }
}

void ClientHandler::handleGetChatHistory(const QString& chatIdStr) {
    qDebug() << "[ACTION] GET_CHAT_HISTORY userId:" << m_userId << "chatId:" << chatIdStr;
    if(!checkAuthorized()) return;
    bool ok;
    int chatId = chatIdStr.toInt(&ok);
    if(!ok) {
        sendToClient(socket, "ERROR|INVALID_CHAT_ID");
        return;
    }
    QString response = m_chatService->getChatHistory(m_userId, chatId);
    sendToClient(socket, response);
}

void ClientHandler::handleFindUsers(const QString& search) {
    qDebug() << "[ACTION] FIND_USERS userId:" << m_userId << "query:" << search.left(20);
    if(!checkAuthorized()) return;
    QString response = m_chatService->findUsers(search);
    sendToClient(socket, response);
}

void ClientHandler::handleGetStats() {
    qDebug() << "[ACTION] GET_STATS userId:" << m_userId;
    if(!checkAuthorized()) return;
    QString response = m_chatService->getStats(m_userId);
    sendToClient(socket, response);
}