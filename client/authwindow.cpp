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
    ClientManager *client = ClientManager::getInstance();
    disconnect(client, &ClientManager::dataReceived, this, &AuthWindow::onDataReceived);
    disconnect(client, &ClientManager::connected, this, &AuthWindow::onConnected);
    delete ui;
}

bool AuthWindow::isAdmin() const {
    return m_isAdmin;
}

void AuthWindow::on_pushButton_login_clicked() {
    m_pendingLogin = ui->lineEdit_login->text().trimmed();
    m_pendingPassword = ui->lineEdit_password->text();
    QString key = ui->lineEdit_cryptoKey->text();

    if(m_pendingLogin.isEmpty() || m_pendingPassword.isEmpty() || key.isEmpty()) {
        QMessageBox::warning(this, "Внимание", "Заполните Логин, Пароль и Ключ шифрования!");
        return;
    }

    ClientManager *client = ClientManager::getInstance();
    m_pendingCommand = QString("LOGIN|%1|%2").arg(m_pendingLogin, m_pendingPassword);
    m_waitingForResponse = true;

    if (client->isConnected()) {
        onConnected();
    } else {
        client->connectToServer("127.0.0.1", 33333);
    }
}

void AuthWindow::on_pushButton_register_clicked() {
    m_pendingLogin = ui->lineEdit_login->text().trimmed();
    m_pendingPassword = ui->lineEdit_password->text();
    QString key = ui->lineEdit_cryptoKey->text();

    if(m_pendingLogin.isEmpty() || m_pendingPassword.isEmpty() || key.isEmpty()) {
        QMessageBox::warning(this, "Внимание", "Заполните Логин, Пароль и Ключ шифрования!");
        return;
    }

    ClientManager *client = ClientManager::getInstance();
    m_pendingCommand = QString("REGISTER|%1|%2").arg(m_pendingLogin, m_pendingPassword);
    m_waitingForResponse = true;

    if (client->isConnected()) {
        onConnected();
    } else {
        client->connectToServer("127.0.0.1", 33333);
    }
}

void AuthWindow::onConnected() {
    if (!m_pendingCommand.isEmpty()) {
        ClientManager::getInstance()->sendSystemMessage(m_pendingCommand);
    }
}

void AuthWindow::onDataReceived(const QByteArray &data) {
    if (!m_waitingForResponse) return;

    QString response = QString::fromUtf8(data);

    if (response.startsWith("OK|LOGIN")) {
        m_waitingForResponse = false;
        ClientManager::getInstance()->setUserName(m_pendingLogin);
        m_isAdmin = (m_pendingLogin.toLower() == "admin");
        m_pendingCommand.clear();
        accept();
    } else if (response.startsWith("OK|REGISTER")) {
        m_waitingForResponse = false;
        QMessageBox::information(this, "Успех", "Регистрация прошла успешно. Теперь войдите.");
        m_pendingCommand.clear();
    } else if (response.startsWith("ERROR|")) {
        m_waitingForResponse = false;
        QString error = response.mid(6);
        if (error.startsWith("USER_EXISTS")) {
            QMessageBox::critical(this, "Ошибка", "Пользователь уже существует!");
        } else if (error.startsWith("REGISTER_ERROR")) {
            QMessageBox::critical(this, "Ошибка", "Ошибка регистрации!");
        } else if (error.startsWith("LOGIN_WRONG_PASSWORD")) {
            QMessageBox::critical(this, "Ошибка", "Неверный пароль!");
        } else if (error.startsWith("LOGIN_USER_NOT_FOUND")) {
            QMessageBox::critical(this, "Ошибка", "Пользователь не найден!");
        } else if (error.startsWith("LOGIN_ERROR")) {
            QMessageBox::critical(this, "Ошибка", "Ошибка авторизации!");
        } else if (error.startsWith("ALREADY_AUTHORIZED")) {
            // Сервер считает нас авторизованными — принимаем
            ClientManager::getInstance()->setUserName(m_pendingLogin);
            m_isAdmin = (m_pendingLogin.toLower() == "admin");
            m_pendingCommand.clear();
            accept();
        } else {
            QMessageBox::critical(this, "Ошибка", "Ошибка: " + error);
        }
        m_pendingCommand.clear();
    }
}

QString AuthWindow::getCryptoKey() const {
    return ui->lineEdit_cryptoKey->text();
}
