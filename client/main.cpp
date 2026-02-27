#include "mainwindow.h"
#include "authwindow.h"
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

    a.setStyleSheet(
        "QToolTip { background-color: #333; color: #fff; border: 1px solid #667eea; }"
        "QScrollBar:vertical { background: transparent; width: 8px; }"
        "QScrollBar::handle:vertical { background: rgba(255,255,255,0.5); border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: rgba(255,255,255,0.7); }"
    );

    AuthWindow auth;

    if (auth.exec() == QDialog::Accepted) {
        ClientManager::getInstance()->setSecretKey(auth.getCryptoKey());

        MainWindow w;

        w.setAdminRole(auth.isAdmin());

        w.show();
        return a.exec();
    }
}
