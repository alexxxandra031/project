#include "clientmanager.h"
#include <QDebug>
#include "crypto.h"

ClientManager* ClientManager::getInstance() {
    static ClientManager instance;
    return &instance;
}

ClientManager::ClientManager(QObject *parent)
    : QObject(parent), m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::connected, this, &ClientManager::connected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientManager::disconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientManager::onReadyRead);

    connect(m_socket, &QTcpSocket::errorOccurred, this, [this](QTcpSocket::SocketError socketError) {
        Q_UNUSED(socketError);
        emit errorOccurred(m_socket->errorString());
    });
}

ClientManager::~ClientManager() {
    if (m_socket->isOpen()) {
        m_socket->close();
    }
}

void ClientManager::connectToServer(const QString &ip, quint16 port)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Connecting to server..." << ip << port;
        m_socket->connectToHost(ip, port);
    }
}

void ClientManager::disconnectFromServer() {
    if (m_socket->isOpen()) {
        m_socket->disconnectFromHost();
    }
}

void ClientManager::sendSystemMessage(const QString &command) {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write(command.toUtf8());
        m_socket->flush();
    }
}

void ClientManager::onReadyRead() {
    QByteArray data = m_socket->readAll();

    if (data.startsWith("NEW_MESSAGE|")) {
        QList<QByteArray> parts = data.split('|');
        if (parts.size() >= 4) {
            QByteArray encryptedMessage = parts[3];
            QByteArray decryptedMessage = Crypto::encryptDecrypt(encryptedMessage, m_secretKey);

            QByteArray result = "NEW_MESSAGE|" + parts[1] + "|" + parts[2] + decryptedMessage;
            emit dataReceived(result);
        } else {
            emit dataReceived(data);
        }
    } else if (data.startsWith("OK|") || data.startsWith("ERROR|")) {
        emit dataReceived(data);
    } else {
        emit dataReceived(Crypto::encryptDecrypt(data, m_secretKey));
        // На всякий случай если вдруг понадобится. (старая логика приема сообщений)
    }
}

void ClientManager::setSecretKey(const QString &key) {
    m_secretKey = key.toUtf8();
}

QString ClientManager::username() const {
    return m_userName;
}

bool ClientManager::isConnected() const {
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void ClientManager::setUserName(const QString &name) {
    m_userName = name;
}

void ClientManager::sendChatMessage(const QString &chatId, const QByteArray &message) {
    if (!this->isConnected()) {
        qDebug() << "Failed to send message: not connected to server.";
        return;
    }

    QByteArray encryptedMessage = Crypto::encryptDecrypt(message, m_secretKey);

    QString command = "SEND|" + chatId + "|";
    QByteArray fullCommand  = command.toUtf8() + encryptedMessage;

    m_socket->write(fullCommand);
    m_socket->flush();
}
