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
    void requestUserInfo();  // новый метод для загрузки данных при старте

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
    bool m_updatingChats = false;  // флаг для предотвращения ложных срабатываний onChatSelected
    void addMessage(const QString &sender, const QString &text, bool isOutgoing);

    void loadChats();
    void switchToChat(int chatId);

    // Хранит имя чата, полученное с сервера (chatId -> chatName)
    QString m_pendingRenameName;  // имя для переименования

    int m_currentChatId = 1;
    QMap<int, QString> m_chats;
};
#endif // MAINWINDOW_H
