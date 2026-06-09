#include "tcpserver.h"
#include "chatservice.h"
#include "clienthandler.h"

tcpServer::~tcpServer() {
    mTcpServer->close();
}

tcpServer::tcpServer(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);
    mChatService = new ChatService();
    connect(mTcpServer, &QTcpServer::newConnection, this, &tcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "server is not started";
    } else {
        qDebug() << "server is started";
    }
}

void tcpServer::slotNewConnection(){
    QTcpSocket *socket = mTcpServer->nextPendingConnection();
    ClientHandler* client = new ClientHandler(socket, this, mChatService);
    mClients.push_back(client);
    connect(socket, &QTcpSocket::readyRead, this, &tcpServer::slotServerRead);
    connect(socket, &QTcpSocket::disconnected, this, &tcpServer::slotClientDisconnected);
    qDebug() << "Новый клиент подключился:" << socket->socketDescriptor();
}

void tcpServer::slotServerRead() {
    QTcpSocket* socket = (QTcpSocket*)sender();
    QByteArray array = socket->readAll();
    QString request = QString::fromUtf8(array).trimmed();
    ClientHandler* client = nullptr;
    for (int i = 0; i < mClients.size(); ++i) {
        if (mClients[i]->getSocket() == socket) {
            client = mClients[i];
            break;
        }
    }
    if(client) {
        client->parseRequest(request);
    }
}

void tcpServer::slotClientDisconnected(){
    QTcpSocket *socket = (QTcpSocket*)sender();
    for (int i = 0; i < mClients.size(); ++i) {
        if (mClients[i]->getSocket() == socket) {
            mClients[i]->handleLogout();
            delete mClients[i];
            mClients.removeAt(i);
        }
    }
    socket->deleteLater();
    qDebug() << "Клиент отключился."<< socket->socketDescriptor();
}