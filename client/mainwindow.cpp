#include "adminwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_isAdmin(false)
{
    ui->setupUi(this);

    ClientManager *client = ClientManager::getInstance();

    connect(client, &ClientManager::connected, this, &MainWindow::onConnected);
    connect(client, &ClientManager::dataReceived, this, &MainWindow::onDataReceived);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnected() {
    ui->textEdit_chat->append("<b>[Система]:</b> Успешное подключение к серверу!");
}

void MainWindow::onDataReceived(const QByteArray &data)
{
    QString raw = QString::fromUtf8(data);

    if (raw.startsWith("AUTH_SUCCESS") || raw.startsWith("STATS|")) {
        return;
    }

    bool isReadable = true;
    for (QChar ch : raw) {
        if (ch.unicode() < 32 && !ch.isSpace()) {
            isReadable = false;
            break;
        }
    }

    if (!isReadable) {
        ui->textEdit_chat->append(
            "<div style='color: #888; font-style: italic; margin:5px 0;'>"
            "[Зашифрованное сообщение: неверный ключ]"
            "</div>"
        );
        return;
    }


    int colonIndex = raw.indexOf(": ");

    if (colonIndex == -1) {
        ui->textEdit_chat->append(raw);
        return;
    }

    QString senderName = raw.left(colonIndex);
    QString messageBody = raw.mid(colonIndex + 2);

    QString escapedName = senderName.toHtmlEscaped();
    QString escapedText = messageBody.toHtmlEscaped();

    // 4. Генерируем HTML
    QString messageHtml = QString(
        "<div style='margin-bottom:12px; text-align:left;'>"
        "<div style='background-color: #f1f0f0; color: #1a202c; padding:8px 12px; "
        "border-radius:18px; max-width:80%; display:inline-block;'>"
        "<b style='color:#2d3748;'>%1:</b><br>"
        "<span>%2</span>"
        "</div></div>"
    ).arg(escapedName, escapedText);

    ui->textEdit_chat->append(messageHtml);
}

void MainWindow::on_pushButton_send_clicked()
{

    QString text = ui->lineEdit_message->text();
    if (text.isEmpty()) return;

    QString myName = ClientManager::getInstance()->username();
    if(myName.isEmpty()) myName = "Я";

    QString escapedText = text.toHtmlEscaped();

    QString myMessageHtml = QString(
        "<div style='margin-bottom:12px; text-align:right;'>"
        "<div style='background-color: #e2e8f0; color: #1a202c; padding:8px 12px; "
        "border-radius:18px; max-width:80%; display:inline-block; text-align:left;'>"
        "<b style='color:#2d3748;'>%1:</b><br>"
        "<span>%2</span>"
        "</div></div>"
    ).arg(myName, escapedText);


    ui->textEdit_chat->append(myMessageHtml);

    ClientManager::getInstance()->sendMessage(text.toUtf8());
    ui->lineEdit_message->clear();
}

void MainWindow::setAdminRole(bool isAdmin) {
    m_isAdmin = isAdmin;

    if(m_isAdmin) {
        this->setWindowTitle("Мессенджер - Режим Админа");
        ui->pushButton_adminPanel->setVisible(true);
    } else {
        this->setWindowTitle("Мессенджер - " + ClientManager::getInstance()->username());
        ui->pushButton_adminPanel->setVisible(false);
    }
}

void MainWindow::on_pushButton_adminPanel_clicked()
{
    AdminWindow adminWin(this);
    adminWin.exec();
}

