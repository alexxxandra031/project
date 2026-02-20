#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ClientManager *client = ClientManager::getInstance();

    connect(client, &ClientManager::connected, this, &MainWindow::onConnected);
    connect(client, &ClientManager::dataReceived, this, &MainWindow::onDataReceived);

    client->connectToServer("127.0.0.1", 333333);
}

MainWindow::~MainWindow()
{
    delete ui;
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

    ui->textEdit_chat->append("<b>[Я]:</b> " + text);

    ClientManager::getInstance()->sendMessage(text.toUtf8());

    ui->lineEdit_message->clear();
}
