#ifndef FINDUSERDIALOG_H
#define FINDUSERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class FindUserDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FindUserDialog(int chatId, QWidget *parent = nullptr);

private slots:
    void onSearchClicked();
    void onShowAllClicked();
    void onDataReceived(const QByteArray &data);

private:
    int m_chatId;
    QLineEdit *m_searchEdit;
    QListWidget *m_resultsList;
    QPushButton *m_searchButton;
    QPushButton *m_showAllButton;
    QPushButton *m_closeButton;
};

#endif // FINDUSERDIALOG_H
