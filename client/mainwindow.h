#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QQueue>
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
    void requestUserInfo();

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
    void onRefreshTimer();
    void processCommandQueue();

    void onToggleTheme();       // <-- обработчик нажатия кнопки темы

private:
    Ui::MainWindow *ui;
    bool m_isAdmin;
    bool m_updatingChats = false;
    void addMessage(const QString &sender, const QString &text, bool isOutgoing);
    void updateChatTitle();
    void enqueueCommand(const QString &cmd);

    QString m_pendingRenameName;
    int m_currentChatId = 1;
    QMap<int, QString> m_chats;

    QTimer *m_refreshTimer;
    QQueue<QString> m_commandQueue;
    QQueue<int> m_pendingChatInfoRequests;
    QTimer *m_commandTimer;

    bool m_darkTheme;                // <-- текущее состояние темы
    QPushButton *m_themeButton;      // <-- указатель на кнопку переключения

    void applyTheme();               // <-- применяет стили ко всему окну
};

#endif // MAINWINDOW_H
