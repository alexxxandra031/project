#include "adminwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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
    connect(ui->comboBox_chats, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onChatSelected);
    connect(ui->pushButton_createChat, &QPushButton::clicked,
            this, &MainWindow::onCreateChatClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnected() {
    addMessage("Система", "Успешное подключение к серверу!", false);
    ClientManager::getInstance()->sendSystemMessage("USER_INFO");
}

void MainWindow::onDataReceived(const QByteArray &data)
{
    QString raw = QString::fromUtf8(data);

    if (raw.startsWith("OK|CREATE_CHAT|")) {
        int chatId = raw.mid(14).toInt();
        // Запрашиваем информацию о новом чате
        ClientManager::getInstance()->sendSystemMessage(QString("CHAT_INFO|%1").arg(chatId));
        return;
    }

    if (raw.startsWith("OK|CHAT_INFO|")) {
        QString jsonData = raw.mid(11);
        QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            int chatId = obj["chatId"].toInt();        // сервер должен включать chatId в JSON
            QString chatName = obj["chatName"].toString();
            m_chats[chatId] = chatName;
            ui->comboBox_chats->addItem(chatName, chatId);
        }
        return;
    }

    if (raw.startsWith("OK|USER_INFO|")) {
        QString jsonData = raw.mid(12);
        QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            QJsonArray chatsArray = obj["chats"].toArray();
            m_chats.clear();
            ui->comboBox_chats->clear();
            for (const QJsonValue &val : chatsArray) {
                int chatId = val.toInt();
                // Запрашиваем информацию о чате, чтобы получить имя
                ClientManager::getInstance()->sendSystemMessage(QString("CHAT_INFO|%1").arg(chatId));
            }
        }
        return;
    }

    if (raw.startsWith("OK|HISTORY|")) {
        QString jsonData = raw.mid(11);
        QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
        if (doc.isArray()) {
            QString current_user = ClientManager::getInstance()->username();
            QJsonArray messages = doc.array();
            for (const QJsonValue &val : messages) {
                QJsonObject obj = val.toObject();
                QString sender = obj["user"].toString();
                QString message = obj["message"].toString();
                addMessage(sender, message, current_user == sender);
            }
        }
        return;
    }

    if (raw.startsWith("ERROR|")) {
        QStringList parts = raw.split('|');
        QString errorType = parts.size() > 1 ? parts[1] : "UNKNOWN";

        if (errorType == "NOT_AUTHORIZED") {
            addMessage("⚠️ Система", "Не авторизован. Перезайдите.", false);
        } else if (errorType == "SEND_FAILED") {
            addMessage("⚠️ Система", "Не удалось отправить сообщение.", false);
        } else {
            addMessage("⚠️ Система", "Ошибка: " + errorType, false);
        }
        return;
    }

    if (raw.startsWith("OK|") || raw.startsWith("ERROR|")) {
        return;
    }

    if (raw.startsWith("NEW_MESSAGE|")) {
        QString jsonPart = raw.mid(12);
        QJsonDocument doc = QJsonDocument::fromJson(jsonPart.toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            QString sender = obj["username"].toString();
            QString message = obj["message"].toString();
        }

        bool isReadable = true;
        for (QChar ch : message) {
            if (ch.unicode() < 32 && !ch.isSpace()) {
                isReadable = false;
                break;
            }
        }

        if (!isReadable) {
            addMessage("⚠️ Система", ("Зашифрованное сообщение: неверный ключ (" + raw + ")"), false);
            return;
        }


        int colonIndex = raw.indexOf(": ");

        if (colonIndex == -1) {
            addMessage("⚠️ Система", raw, false);
            return;
        }

        addMessage(sender, message, false);
        return;
    }
}

void MainWindow::on_pushButton_send_clicked()
{
    QString text = ui->lineEdit_message->text();
    if (text.isEmpty()) return;

    QString myName = ClientManager::getInstance()->username();
    if(myName.isEmpty()) myName = "Я";

    addMessage(myName, text, true);
    ClientManager::getInstance()->sendChatMessage(QString::number(m_currentChatId), text.toUtf8());
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

void MainWindow::onCreateChatClicked() {
    ClientManager::getInstance()->sendSystemMessage("CREATE_CHAT");
}

void MainWindow::onChatSelected(int index) {
    if (index < 0) return;
    int chatId = ui->comboBox_chats->itemData(index).toInt();
    if (chatId == m_currentChatId) return;
    m_currentChatId = chatId;
    // Очищаем область сообщений
    ui->listWidget_chat->clear();
    // Загружаем историю выбранного чата
    ClientManager::getInstance()->sendSystemMessage(QString("HISTORY|%1").arg(chatId));
}
