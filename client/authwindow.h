#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QDialog>

namespace Ui {class AuthWindow;}

class AuthWindow: public QDialog {
    Q_OBJECT
public:
    explicit AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();

    QString getCryptoKey() const;

    bool isAdmin() const;

private slots:
    void on_pushButton_login_clicked();

private:
        Ui::AuthWindow *ui;
        bool m_isAdmin = false;
};

#endif // AUTHWINDOW_H
