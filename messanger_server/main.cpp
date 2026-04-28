#include <QCoreApplication>
#include "tcpserver.h"
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    DatabaseManager::getInstance()->connect();
    tcpServer myserv;
    return a.exec();
}