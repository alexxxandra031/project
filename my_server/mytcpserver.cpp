#include "mytcpserver.h"
#include <QDebug>
#include <QCoreApplication>
#include<QString>

MyTcpServer::~MyTcpServer()
{

    mTcpServer->close();
    //server_status=0;
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "server is not started";
    } else {
        //server_status=1;
        qDebug() << "server is started";
    }
}

void MyTcpServer::slotNewConnection(){
    //   if(server_status==1){
    mTcpSocket = mTcpServer->nextPendingConnection();
    connect(mTcpSocket, &QTcpSocket::readyRead,this,&MyTcpServer::slotServerRead);
    connect(mTcpSocket,&QTcpSocket::disconnected,this,&MyTcpServer::slotClientDisconnected);
    mTcpSocket->write("Hello, World!!! I am echo server!\r\n");
    // }
}

void MyTcpServer::slotServerRead() {
    QByteArray array = mTcpSocket->readAll();
    QString request = QString::fromUtf8(array);

    qDebug() << "Получено:" << request;

    QString response = parseRequest(request);


    mTcpSocket->write(response.toUtf8());
}

void MyTcpServer::slotClientDisconnected(){
    mTcpSocket->close();
}

QString MyTcpServer::parseRequest(const QString& request) {
    QStringList parts = request.split(";");


    if(parts.size() == 0)
        return "ERROR";

    QString command = parts[0];

    if(command == "LOGIN" && parts.size() == 3)
    {
        return loginUser(parts[1], parts[2]);
    }

    if(command == "REGISTER" && parts.size() == 3)
    {
        return registerUser(parts[1], parts[2]);
    }

    if(command == "SEND" && parts.size() == 4)
    {
        return handleSendMessage(parts[1], parts[2], parts[3]);
    }

    if(command == "STATUS" && parts.size() == 3)
    {
        return setUserStatus(parts[1], parts[2]);
    }

    if(command == "GET_HISTORY" && parts.size() == 3)
    {
        return getChatHistory(parts[1], parts[2]);
    }

    return "Неопознанная команда";
}

//заглушка для метода входа пользователя
QString MyTcpServer::loginUser(const QString& login, const QString& password) {
    qDebug() << "Попытка входа:" << login << password;
    return "Вход выполнен успешно";
}

//заглушка для метода регистрации пользователя
QString MyTcpServer::registerUser(const QString& login, const QString& password) {
    qDebug() << "Попытка регистрации:" << login << password;
    return "Регистрация выполнена успешно";
}

//заглушка для метода отправки сообщения
QString MyTcpServer::handleSendMessage(const QString& sender, const QString& receiver, const QString& message) {
    qDebug() << "Обработка отправки сообщения: "<< message << "от" << sender << "к" << receiver;

    bool savingMessage = saveMessage(sender, receiver, message);

    if(!savingMessage) return "Ошибка сохранения сообщения";

    deliverMessage(sender, receiver, message);

    return "Сообщение принято сервером";
}

//заглушка для метода сохранения сообщение в бд
bool MyTcpServer::saveMessage(const QString& sender, const QString& receiver, const QString& message) {
    qDebug() << "Сохранение сообщения в бд:" << sender << "|" << receiver << "|" << message;

    //сохранение в бд

    return true; //заглушка
}

//заглушка для метода отправки сообщения отправителю
void MyTcpServer::deliverMessage(const QString& sender, const QString& receiver, const QString& message) {
    qDebug() << "Попытка доставки сообщения: "<< message << "от" << sender << "к" << receiver;

}

//заглушка для метода обозначения статуса пользователя(онлайн/оффлайн)
QString MyTcpServer::setUserStatus(const QString& user, const QString& status) {
    qDebug() << "Установка статуса:" << user << status;
    return "Статус пользователя был успешно сменен";
}


QString MyTcpServer::getChatHistory(const QString& user1, const QString& user2)
{
    qDebug() << "Запрос истории чата между:" << user1 << "и" << user2;

    //пока заглушка, позже будет осуществляться доступ к бд
    return "HISTORY;Сообщение1;Сообщение2?;Сообщение3";
}


