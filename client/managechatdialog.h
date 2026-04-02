#ifndef MANAGECHATDIALOG_H
#define MANAGECHATDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "clientmanager.h"

class ManageChatDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ManageChatDialog(int chatId, const QString &chatName, QWidget *parent = nullptr);

private slots:
    void onAddUserClicked();
    void onDataReceived(const QByteArray &data);
    void loadParticipants();

private:
    int m_chatId;
    QString m_chatName;
    QListWidget *m_participantsList;
    QLineEdit *m_userNameEdit;
    QPushButton *m_addButton;
    QPushButton *m_closeButton;
};

#endif // MANAGECHATDIALOG_H
