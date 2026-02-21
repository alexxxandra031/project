#include "authwindow.h"
#include "ui_authwindow.h"
#include <QMessageBox>

AuthWindow::AuthWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuthWindow)
{
    ui->setupUi(this);
}

AuthWindow::~AuthWindow()
{
    delete ui;
}

bool AuthWindow::isAdmin() const {
    return m_isAdmin;
}

void AuthWindow::on_pushButton_login_clicked() {
    QString login = ui->lineEdit_login->text();
    QString password = ui->lineEdit_password->text();

    if(login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин и пароль!");
        return;
    }

    // ЗАГЛУШКА
    // ClientManager::getInstance()->sendMessage("AUTH|" + login + "|" + password);

    if(login == "admin" && password == "123") {
            m_isAdmin = true;
            accept();
        } else {
            m_isAdmin = false;
            accept();
        }
}
