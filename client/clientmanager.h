#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include <QTcpSocket>

class ClientManager : public QObject {
    Q_OBJECT

public:
    static ClientManager* getInstance();

    ClientManager(const ClientManager&) = delete;
    ClientManager& operator=(const ClientManager&) = delete;

    void connectToServer(const QString &ip, quint16 port);
    void disconnectFromServer();
    void sendMessage(const QByteArray &message);
    void setSecretKey(const QString &key);
    void sendSystemMessage(const QString &command);

signals:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &errorString);

private slots:
    void onReadyRead();

private:
    explicit ClientManager(QObject *parent = nullptr);
    ~ClientManager();

    QByteArray m_secretKey;

    QTcpSocket *m_socket;
};

#endif // CLIENTMANAGER_H
