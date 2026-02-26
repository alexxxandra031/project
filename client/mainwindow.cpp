#include "adminwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_isAdmin(false)
{
    ui->setupUi(this);

    ui->pushButton_markImportant->setCheckable(true);
    ui->pushButton_markImportant->setText("Отметить как важное");

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

void MainWindow::onDataReceived(const QByteArray &data)
{
    QString raw = QString::fromUtf8(data);

    // Простейшая проверка на совпадение ключей
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

    bool isImportant = raw.startsWith("[ВАЖНО] ");
    QString cleanText = isImportant ? raw.mid(8) : raw;
    cleanText = cleanText.toHtmlEscaped();

    QString messageHtml;
    if (isImportant) {
        messageHtml = QString(
            "<div style='margin-bottom:12px; text-align:left;'>"
            "<div style='background-color: #fed7d7; border: 2px solid #f56565; color: #1a202c; padding:8px 12px; "
            "border-radius:18px; max-width:80%%; display:inline-block;'>"
            "<b style='color:#c53030;'>❗ [ВАЖНО] Собеседник</b><br>"
            "<span>%1</span>"
            "</div></div>"
        ).arg(cleanText);
    } else {
        messageHtml = QString(
            "<div style='margin-bottom:12px; text-align:left;'>"
            "<div style='background-color: #f1f0f0; color: #1a202c; padding:8px 12px; "
            "border-radius:18px; max-width:80%%; display:inline-block;'>"
            "<b style='color:#2d3748;'>Собеседник:</b><br>"
            "<span>%1</span>"
            "</div></div>"
        ).arg(cleanText);
    }

    ui->textEdit_chat->append(messageHtml);
}

void MainWindow::on_pushButton_send_clicked()
{
    QString text = ui->lineEdit_message->text();
    if (text.isEmpty())
        return;

    QString escapedText = text.toHtmlEscaped();

    QString myMessageHtml;
    if (ui->pushButton_markImportant->isChecked()) {
        // Важное сообщение — красная рамка и фон
        myMessageHtml = QString(
            "<div style='margin-bottom:12px; text-align:right;'>"
            "<div style='background-color: #fed7d7; border: 2px solid #f56565; color: #1a202c; padding:8px 12px; "
            "border-radius:18px; max-width:80%%; display:inline-block; text-align:left;'>"
            "<b style='color:#c53030;'>❗ [ВАЖНО] Я:</b><br>"
            "<span>%1</span>"
            "</div></div>"
        ).arg(escapedText);
    } else {
        // Обычное сообщение
        myMessageHtml = QString(
            "<div style='margin-bottom:12px; text-align:right;'>"
            "<div style='background-color: #e2e8f0; color: #1a202c; padding:8px 12px; "
            "border-radius:18px; max-width:80%%; display:inline-block; text-align:left;'>"
            "<b style='color:#2d3748;'>Я:</b><br>"
            "<span>%1</span>"
            "</div></div>"
        ).arg(escapedText);
    }

    ui->textEdit_chat->append(myMessageHtml);

    QString messageToSend = text;
    if (ui->pushButton_markImportant->isChecked()) {
        messageToSend = "[ВАЖНО] " + messageToSend;   // Добавляем префикс для получателя
    }

    ClientManager::getInstance()->sendMessage(messageToSend.toUtf8());
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

