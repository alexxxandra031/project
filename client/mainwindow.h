#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "clientmanager.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setAdminRole(bool isAdmin);

private slots:
    void on_pushButton_send_clicked();

    void onConnected();
    void onDataReceived(const QByteArray &data);

    void on_pushButton_adminPanel_clicked();

    void onCreateChatClicked();
    void onChatSelected(int index);

    void onManageChatClicked();
    void onLeaveChatClicked();

    void onRenameChatClicked();
    void onLogoutClicked();

private:
    Ui::MainWindow *ui;
    bool m_isAdmin;
    void addMessage(const QString &sender, const QString &text, bool isOutgoing);

    void loadChats();
    void switchToChat(int chatId);

    int m_currentChatId = 1;
    QMap<int, QString> m_chats;
};
#endif // MAINWINDOW_H
