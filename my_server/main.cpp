#include <QCoreApplication>
#include "tcpserver.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    tcpServer myserv;
    return a.exec();
}
