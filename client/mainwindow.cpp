#include "adminwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_isAdmin(false)
{
    ui->setupUi(this);

    ui->pushButton_markImportant->setCheckable(true);
    ui->pushButton_markImportant->setText("Пометить как важное");

    ClientManager *client = ClientManager::getInstance();

    connect(client, &ClientManager::connected, this, &MainWindow::onConnected);
    connect(client, &ClientManager::dataReceived, this, &MainWindow::onDataReceived);

    client->connectToServer("127.0.0.1", 33333);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnected() {
    ui->textEdit_chat->append("<b>[Система]:</b> Успешное подключение к серверу!");
}

void MainWindow::onDataReceived(const QByteArray &data) {
    QString message = QString::fromUtf8(data);
    ui->textEdit_chat->append("<b>[Сервер]:</b> " + message);
}

void MainWindow::on_pushButton_send_clicked()
{
    QString text = ui->lineEdit_message->text();

    if ( text.isEmpty())
        return;

    QString finalMessage;

    text = "<b>[Я]:</b> " + text;

    if (ui->pushButton_markImportant->isChecked()) {
        text = "<font color='red'><b>[ВАЖНО]</b></font> " + text;
    }

    ui->textEdit_chat->append(text.toUtf8());

    ClientManager::getInstance()->sendMessage(text.toUtf8());

    ui->lineEdit_message->clear();

    ui->pushButton_markImportant->setChecked(false);
}

void MainWindow::setAdminRole(bool isAdmin) {
    m_isAdmin = isAdmin;

    if(m_isAdmin) {
        this->setWindowTitle("Мессенджер - Режим Админа");
        ui->pushButton_adminPanel->setVisible(true);
    } else {
        this->setWindowTitle("Мессенджер - Пользователь");
        ui->pushButton_adminPanel->setVisible(false);
    }
}

void MainWindow::on_pushButton_adminPanel_clicked()
{
    AdminWindow adminWin(this);
    adminWin.exec();
}

