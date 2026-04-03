#include "adminwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "managechatdialog.h"
#include "authwindow.h"
#include <QApplication>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_isAdmin(false)
{
    ui->setupUi(this);

    ClientManager *client = ClientManager::getInstance();

    connect(client, &ClientManager::connected,
            this, &MainWindow::onConnected);
    connect(client, &ClientManager::dataReceived,
            this, &MainWindow::onDataReceived);
    connect(ui->lineEdit_message, &QLineEdit::returnPressed,
            this, &MainWindow::on_pushButton_send_clicked);
    connect(ui->comboBox_chats, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onChatSelected);
    connect(ui->pushButton_createChat, &QPushButton::clicked,
            this, &MainWindow::onCreateChatClicked);
    connect(ui->pushButton_manageChat, &QPushButton::clicked,
            this, &MainWindow::onManageChatClicked);
    connect(ui->pushButton_renameChat, &QPushButton::clicked,
            this, &MainWindow::onRenameChatClicked);
    connect(ui->pushButton_logout, &QPushButton::clicked,
            this, &MainWindow::onLogoutClicked);

    // Сразу запрашиваем данные, т.к. соединение уже установлено после AuthWindow
    requestUserInfo();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::requestUserInfo()
{
    if (ClientManager::getInstance()->isConnected()) {
        ClientManager::getInstance()->sendSystemMessage("USER_INFO");
    }
}

void MainWindow::onConnected() {
    addMessage("Система", "Успешное подключение к серверу!", false);
    requestUserInfo();
}

void MainWindow::onDataReceived(const QByteArray &data)
{
    QString raw = QString::fromUtf8(data);

    if (raw.startsWith("OK|LOGOUT")) {
        QMessageBox::information(this, "Успех", "Вы вышли из системы");
        ClientManager::getInstance()->disconnectFromServer();
        ClientManager::getInstance()->setUserName("");
        m_chats.clear();
        m_updatingChats = true;
        ui->comboBox_chats->clear();
        m_updatingChats = false;
        ui->listWidget_chat->clear();
        this->close();
        AuthWindow auth;
        if (auth.exec() == QDialog::Accepted) {
            ClientManager::getInstance()->setSecretKey(auth.getCryptoKey());
            setAdminRole(auth.isAdmin());
            // После логина соединение уже есть, запрашиваем инфу
            requestUserInfo();
            this->show();
        } else {
            QApplication::quit();
        }
        return;
    }

    if (raw.startsWith("OK|ADD_USER")) {
        addMessage("Система", "Пользователь добавлен в чат", false);
        return;
    }

    if (raw.startsWith("OK|CHANGE_CHAT_NAME")) {
        QMessageBox::information(this, "Успех", "Название чата изменено");
        // Обновляем название в comboBox и в карте
        if (!m_pendingRenameName.isEmpty()) {
            int idx = ui->comboBox_chats->currentIndex();
            if (idx != -1) {
                ui->comboBox_chats->setItemText(idx, m_pendingRenameName);
                m_chats[m_currentChatId] = m_pendingRenameName;
            }
            m_pendingRenameName.clear();
        }
        return;
    }

    if (raw.startsWith("OK|LEAVE_CHAT")) {
        QMessageBox::information(this, "Успех", "Вы покинули чат");
        requestUserInfo();
        return;
    }

    if (raw.startsWith("OK|CREATE_CHAT|")) {
        int chatId = raw.mid(15).toInt();
        QString chatName = "Новый чат";
        m_chats[chatId] = chatName;
        m_updatingChats = true;
        ui->comboBox_chats->addItem(chatName, chatId);
        m_updatingChats = false;
        ui->comboBox_chats->setCurrentIndex(ui->comboBox_chats->count() - 1);
        return;
    }

    if (raw.startsWith("OK|CHAT_INFO|")) {
        // Обработка информации о чате (используется в ManageChatDialog)
        return;
    }

    if (raw.startsWith("OK|USER_INFO|")) {
        QString jsonData = raw.mid(13);
        QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            QJsonArray chatsArray = obj["chats"].toArray();

            m_chats.clear();
            m_updatingChats = true;
            ui->comboBox_chats->clear();

            // Глобальный чат (id=1)
            m_chats[1] = "Global Chat";
            ui->comboBox_chats->addItem("Global Chat", 1);

            for (const QJsonValue &val : chatsArray) {
                int chatId = val.toInt();
                if (chatId == 1) continue;
                QString chatName = "Чат " + QString::number(chatId);
                m_chats[chatId] = chatName;
                ui->comboBox_chats->addItem(chatName, chatId);
            }

            m_updatingChats = false;

            // Выбираем текущий чат или первый
            int idx = ui->comboBox_chats->findData(m_currentChatId);
            if (idx != -1) {
                ui->comboBox_chats->setCurrentIndex(idx);
            } else {
                m_currentChatId = 1;
                ui->comboBox_chats->setCurrentIndex(0);
            }

            // Загружаем историю текущего чата
            ui->listWidget_chat->clear();
            ClientManager::getInstance()->sendSystemMessage(
                QString("HISTORY|%1").arg(m_currentChatId));
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
        } else if (errorType == "CANNOT_RENAME_GLOBAL_CHAT") {
            QMessageBox::warning(this, "Ошибка", "Нельзя переименовать глобальный чат");
        } else if (errorType == "CHANGE_NAME_FAILED") {
            QMessageBox::warning(this, "Ошибка", "Не удалось изменить название чата");
        } else if (errorType == "LOGOUT_ERROR") {
            QMessageBox::warning(this, "Ошибка", "Не удалось выйти из системы");
        } else if (errorType == "CREATE_CHAT_FAILED") {
            QMessageBox::warning(this, "Ошибка", "Не удалось создать чат");
        } else if (errorType == "ALREADY_AUTHORIZED") {
            // Игнорируем — это может прийти при повторном логине
        } else {
            addMessage("⚠️ Система", "Ошибка: " + errorType, false);
        }
        return;
    }

    if (raw.startsWith("OK|")) {
        // Другие OK-ответы — игнорируем
        return;
    }

    if (raw.startsWith("NEW_MESSAGE|")) {
        QString jsonPart = raw.mid(12);
        QJsonDocument doc = QJsonDocument::fromJson(jsonPart.toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            int chatId = obj["chatId"].toInt();
            QString sender = obj["username"].toString();
            QString message = obj["message"].toString();
            QString currentUser = ClientManager::getInstance()->username();

            // Показываем только если это текущий чат и не от нас
            if (chatId == m_currentChatId && sender != currentUser) {
                addMessage(sender, message, false);
            }
        }
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
    if (m_updatingChats) return;  // предотвращаем ложные срабатывания
    if (index < 0) return;

    int chatId = ui->comboBox_chats->itemData(index).toInt();
    if (chatId == m_currentChatId) return;

    m_currentChatId = chatId;
    // Очищаем область сообщений
    ui->listWidget_chat->clear();
    // Загружаем историю выбранного чата
    ClientManager::getInstance()->sendSystemMessage(QString("HISTORY|%1").arg(chatId));
}

void MainWindow::onManageChatClicked()
{
    QString chatName = ui->comboBox_chats->currentText();
    ManageChatDialog dialog(m_currentChatId, chatName, this);
    dialog.exec();
}

void MainWindow::onLeaveChatClicked()
{
    if (m_currentChatId == 1) {
        QMessageBox::warning(this, "Внимание", "Нельзя покинуть общий чат");
        return;
    }
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Выход из чата",
        "Вы уверены, что хотите покинуть чат \"" + ui->comboBox_chats->currentText() + "\"?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ClientManager::getInstance()->sendSystemMessage(QString("LEAVE_CHAT|%1").arg(m_currentChatId));
    }
}

void MainWindow::onRenameChatClicked()
{
    if (m_currentChatId == 1) {
        QMessageBox::warning(this, "Внимание", "Нельзя переименовать общий чат");
        return;
    }
    bool ok;
    QString newName = QInputDialog::getText(this, "Переименование чата",
                                            "Введите новое название чата:",
                                            QLineEdit::Normal,
                                            ui->comboBox_chats->currentText(),
                                            &ok);
    if (ok && !newName.isEmpty()) {
        m_pendingRenameName = newName;  // сохраняем для обработки ответа
        ClientManager::getInstance()->sendSystemMessage(
            QString("CHANGE_CHAT_NAME|%1|%2").arg(m_currentChatId).arg(newName));
    }
}

void MainWindow::onLogoutClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Выход",
        "Вы уверены, что хотите выйти из аккаунта?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ClientManager::getInstance()->sendSystemMessage("LOGOUT");
    }
}
