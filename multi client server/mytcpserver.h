#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include <QtNetwork>
#include <QByteArray>
#include <QDebug>
#include <QList>



class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();
public slots:
    void slotNewConnection();
    void slotClientDisconnected();
    void slotServerRead();
private:
    QTcpServer * mTcpServer;
    QTcpSocket * mTcpSocket;
    QList<QTcpSocket*> mTcpSockets;
    QString parseRequest(const QString& request);
    QString loginUser(const QString& login, const QString& password);
    QString registerUser(const QString& login, const QString& password);
    QString handleSendMessage(const QString& sender, const QString& receiver, const QString& message);
    QString setUserStatus(const QString& user, const QString& status);
    bool saveMessage(const QString& sender,const QString& receiver, const QString& message);
    void deliverMessage(const QString& sender,const QString& receiver, const QString& message);
    QString getChatHistory(const QString& user1, const QString& user2);
    //int server_status;
};
#endif // MYTCPSERVER_H



