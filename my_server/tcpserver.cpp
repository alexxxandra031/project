#include "tcpserver.h"
#include "chatservice.h"
#include "clienthandler.h"
#include <QDebug>
#include <QCoreApplication>
#include<QString>

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
    QString request = QString::fromUtf8(array);
    qDebug() << "Получено:" << request;

    ClientHandler* client = nullptr;
    for (int i = 0; i < mClients.size(); ++i) {
        if (mClients[i]->getSocket() == socket) {
            client = mClients[i];
            break;
        }
    }

    if(client) {
        QString response = client->parseRequest(request);
        socket->write(response.toUtf8());
    }
}


void tcpServer::slotClientDisconnected(){
    QTcpSocket *socket = (QTcpSocket*)sender();

    for (int i = 0; i < mClients.size(); ++i) {
        if (mClients[i]->getSocket() == socket) {
            delete mClients[i];
            mClients.removeAt(i);
        }
    }
    socket->deleteLater();
    qDebug() << "Клиент отключился.";
}



