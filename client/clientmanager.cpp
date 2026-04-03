#include "clientmanager.h"
#include <QDebug>
#include "crypto.h"
#include <QJsonDocument>
#include <QJsonObject>

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
        qDebug() << "[CLIENT SEND]" << command;
        m_socket->write(command.toUtf8());
        m_socket->flush();
    } else {
        qDebug() << "[CLIENT ERROR] Not connected, cannot send:" << command;
    }
}

void ClientManager::onReadyRead() {
    // Читаем все данные и обрабатываем возможные склеенные пакеты
    m_buffer.append(m_socket->readAll());

    // Простая обработка: каждое сообщение от сервера — отдельная строка
    // Но сервер не использует разделители, поэтому обрабатываем буфер целиком
    QByteArray data = m_buffer;
    m_buffer.clear();

    if (data.isEmpty()) return;

    qDebug() << "[CLIENT RECV]" << QString::fromUtf8(data).left(200);

    if (data.startsWith("NEW_MESSAGE|")) {
        QByteArray jsonPart = data.mid(12);
        QJsonDocument doc = QJsonDocument::fromJson(jsonPart);
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            QString base64Message = obj["message"].toString();
            QByteArray encryptedMessage = Crypto::fromBase64(base64Message.toUtf8());
            QByteArray decryptedMessage = Crypto::encryptDecrypt(encryptedMessage, m_secretKey);
            obj["message"] = QString::fromUtf8(decryptedMessage);
            QJsonDocument newDoc(obj);
            QByteArray result = "NEW_MESSAGE|" + newDoc.toJson(QJsonDocument::Compact);
            emit dataReceived(result);
        } else {
            emit dataReceived(data);
        }
    } else if (data.startsWith("OK|") || data.startsWith("ERROR|")) {
        emit dataReceived(data);
    } else {
        emit dataReceived(Crypto::encryptDecrypt(data, m_secretKey));
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
    QByteArray base64Message = Crypto::toBase64(encryptedMessage);

    QString command = "SEND|" + chatId + "|";
    QByteArray fullCommand = command.toUtf8() + base64Message;

    qDebug() << "[CLIENT SEND MSG]" << command << "base64len:" << base64Message.size();
    m_socket->write(fullCommand);
    m_socket->flush();
}
