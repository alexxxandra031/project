#include "mainwindow.h"
#include "authwindow.h"
#include "thememanager.h"
#include <QApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    QApplication a(argc, argv);

    ThemeManager::loadSettings();

    a.setStyleSheet(ThemeManager::globalStyleSheet());

    AuthWindow auth;

    auth.setStyleSheet(ThemeManager::authWindowStyle());

    if (auth.exec() == QDialog::Accepted) {
        ClientManager::getInstance()->setSecretKey(auth.getCryptoKey());

        MainWindow w;
        w.setAdminRole(auth.isAdmin());
        w.show();

        return a.exec();
    }

    return 0;
}
