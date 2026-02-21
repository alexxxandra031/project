#include "mainwindow.h"
#include "authwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AuthWindow auth;

    if (auth.exec() == QDialog::Accepted) {
        MainWindow w;

        w.setAdminRole(auth.isAdmin());

        w.show();
        return a.exec();
    }
}
