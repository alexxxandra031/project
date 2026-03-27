#include "authwindow.h"
#include "ui_authwindow.h"
#include <QMessageBox>
#include "clientmanager.h"

AuthWindow::AuthWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuthWindow)
{
    ui->setupUi(this);

    ClientManager *client = ClientManager::getInstance();

    connect(client, &ClientManager::dataReceived, this, &AuthWindow::onDataReceived);
    connect(client, &ClientManager::connected, this, &AuthWindow::onConnected);
}

AuthWindow::~AuthWindow()
{
    delete ui;
}

bool AuthWindow::isAdmin() const {
    return m_isAdmin;
}

void AuthWindow::on_pushButton_login_clicked() {
    m_pendingLogin = ui->lineEdit_login->text();
    m_pendingPassword = ui->lineEdit_password->text();
    QString key = ui->lineEdit_cryptoKey->text();

    if(m_pendingLogin.isEmpty() || m_pendingPassword.isEmpty() || key.isEmpty()) {
        QMessageBox::warning(this, "Внимание", "Заполните Логин, Пароль и Ключ шифрования!");
        return;
    }


    ClientManager *client = ClientManager::getInstance();

    if (client->isConnected()) {
        onConnected();
    } else {
        client->connectToServer("127.0.0.1", 33333);
    }

}

void AuthWindow::onDataReceived(const QByteArray &data) {
    QString response = QString::fromUtf8(data);
    if (response.startsWith("OK|LOGIN")) {
        ClientManager::getInstance()->setUserName(m_pendingLogin);
        m_isAdmin = (m_pendingLogin == "admin");
        accept();
    } else if (response.startsWith("ERROR|")) {
        QMessageBox::critical(this, "Ошибка", "Неверный логин или пароль!");
    }


}

void AuthWindow::onConnected() {
    QString authmsg = "LOGIN|" + m_pendingLogin + "|" + m_pendingPassword;
    ClientManager::getInstance()->sendSystemMessage(authmsg);

}

QString AuthWindow::getCryptoKey() const {
    return ui->lineEdit_cryptoKey->text();
}
