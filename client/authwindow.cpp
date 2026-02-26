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

    if(ui->lineEdit_login->text().isEmpty() || ui->lineEdit_cryptoKey->text().isEmpty()) {
        QMessageBox::warning(this, "Внимание", "Заполните Логин, Пароль и Ключ шифрования!");
        return;
    }


    // ЗАГЛУШКА
    // ClientManager::getInstance()->connectToServer("127.0.0.1", 33333);
    // ClientManager::getInstance()->sendSystemMessage("AUTH|" + login + "|" + password);

    if(login == "admin" && password == "123") {
            m_isAdmin = true;
            accept();
    } else {
        m_isAdmin = false;
        accept();
    }
}

QString AuthWindow::getCryptoKey() const {
    return ui->lineEdit_cryptoKey->text();
}
