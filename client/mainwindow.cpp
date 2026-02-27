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
    connect(ui->lineEdit_message,
            &QLineEdit::returnPressed,
            this,
            &MainWindow::on_pushButton_send_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnected() {
    addMessage("Система", "Успешное подключение к серверу!", false);
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
        addMessage("⚠️ Система", "Зашифрованное сообщение: неверный ключ", false);
        return;
    }


    int colonIndex = raw.indexOf(": ");

    if (colonIndex == -1) {
        addMessage("⚠️ Система", raw, false);
        return;
    }

    QString senderName = raw.left(colonIndex);
    QString messageBody = raw.mid(colonIndex + 2);
    addMessage(senderName, messageBody, false);
}

void MainWindow::on_pushButton_send_clicked()
{

    QString text = ui->lineEdit_message->text();
    if (text.isEmpty()) return;

    QString myName = ClientManager::getInstance()->username();
    if(myName.isEmpty()) myName = "Я";

    addMessage(myName, text, true);

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

void MainWindow::addMessage(const QString &sender, const QString &text, bool isOutgoing) {
    QWidget *container = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(container);
    layout->setContentsMargins(5, 2, 5, 2);


    QFrame *bubble = new QFrame();
    QString bgColor = isOutgoing ? "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2)" : "#f1f3f5";
    QString textColor = isOutgoing ? "white" : "#2d3748";
    QString senderColor = isOutgoing ? "#e0e7ff" : "#764ba2";

    bubble->setStyleSheet(
        "QFrame {"
        "   background-color: " + bgColor + ";"
        "   border-radius: 12px;"
        "   padding: 6px 10px;"
        "}"
    );

    QLabel *label = new QLabel();
    label->setTextFormat(Qt::RichText);
    label->setWordWrap(true);

    label->setMaximumWidth(this->width() * 0.75);

    label->setText(
        "<b style='color:" + senderColor + "; font-size: 11px;'>" + sender + "</b><br>"
        "<span style='color:" + textColor + "; font-size: 14px; font-family: Segoe UI, sans-serif;'>" + text + "</span>"
    );

    label->setStyleSheet("background: transparent; border: none;");

    QVBoxLayout *bubbleLayout = new QVBoxLayout(bubble);
    bubbleLayout->setContentsMargins(0, 0, 0, 0);
    bubbleLayout->addWidget(label);

    if (isOutgoing) {
        layout->addStretch();
        layout->addWidget(bubble);
    } else {
        layout->addWidget(bubble);
        layout->addStretch();
    }

    QListWidgetItem *item = new QListWidgetItem(ui->listWidget_chat);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    container->setLayout(layout);
    item->setSizeHint(container->sizeHint());
    ui->listWidget_chat->addItem(item);
    ui->listWidget_chat->setItemWidget(item, container);

    ui->listWidget_chat->scrollToBottom();
}
