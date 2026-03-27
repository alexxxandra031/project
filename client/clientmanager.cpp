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

void ClientManager::sendMessage(const QByteArray &message) {
    if (this->isConnected()) {
        QString fullMessage = m_userName + ": " + QString::fromUtf8(message);

        QByteArray encryptedData = Crypto::encryptDecrypt(fullMessage.toUtf8(), m_secretKey);

        m_socket->write(encryptedData);
        m_socket->flush();
    } else {
        qDebug() << "Failed to send message: not connected to server.";
    }
}

void ClientManager::onReadyRead() {
    QByteArray data = m_socket->readAll();
    if (data.startsWith("OK|") || data.startsWith("ERROR|")) {
        emit dataReceived(data);
    } else {
        QByteArray decryptedData = Crypto::encryptDecrypt(data, m_secretKey);
        emit dataReceived(decryptedData);
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
