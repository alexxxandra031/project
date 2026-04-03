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

    // Таймер для периодического обновления списка чатов (синхронизация между клиентами)
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &MainWindow::onRefreshTimer);
    m_refreshTimer->start(5000); // каждые 5 секунд

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

void MainWindow::updateChatTitle()
{
    QString chatName = m_chats.value(m_currentChatId, "Чат");
    ui->label_chatTitle->setText("<h2>💬 " + chatName + "</h2>");
}

void MainWindow::onRefreshTimer()
{
    // Периодически запрашиваем USER_INFO для синхронизации списка чатов
    if (ClientManager::getInstance()->isConnected()) {
        ClientManager::getInstance()->sendSystemMessage("USER_INFO");
    }
}

void MainWindow::onDataReceived(const QByteArray &data)
{
    QString raw = QString::fromUtf8(data);

    if (raw.startsWith("OK|LOGOUT")) {
        m_refreshTimer->stop();
        QMessageBox::information(this, "Успех", "Вы вышли из системы");
        // НЕ отключаемся от сервера — просто закрываем окно и показываем авторизацию
        ClientManager::getInstance()->setUserName("");
        m_chats.clear();
        m_updatingChats = true;
        ui->comboBox_chats->clear();
        m_updatingChats = false;
        ui->listWidget_chat->clear();
        m_currentChatId = 1;
        this->close();
        AuthWindow auth;
        if (auth.exec() == QDialog::Accepted) {
            ClientManager::getInstance()->setSecretKey(auth.getCryptoKey());
            setAdminRole(auth.isAdmin());
            m_refreshTimer->start(5000);
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
        if (!m_pendingRenameName.isEmpty()) {
            int idx = ui->comboBox_chats->currentIndex();
            if (idx != -1) {
                ui->comboBox_chats->setItemText(idx, m_pendingRenameName);
                m_chats[m_currentChatId] = m_pendingRenameName;
                updateChatTitle();
            }
            m_pendingRenameName.clear();
        }
        return;
    }

    if (raw.startsWith("OK|LEAVE_CHAT")) {
        QMessageBox::information(this, "Успех", "Вы покинули чат");
        m_currentChatId = 1;
        requestUserInfo();
        return;
    }

    if (raw.startsWith("OK|CREATE_CHAT|")) {
        int chatId = raw.mid(15).toInt();
        QString chatName = "Чат " + QString::number(chatId);
        m_chats[chatId] = chatName;
        m_updatingChats = true;
        ui->comboBox_chats->addItem(chatName, chatId);
        m_updatingChats = false;
        ui->comboBox_chats->setCurrentIndex(ui->comboBox_chats->count() - 1);
        return;
    }

    if (raw.startsWith("OK|CHAT_INFO|")) {
        // Извлекаем имя чата из ответа CHAT_INFO и обновляем comboBox
        QString jsonData = raw.mid(13);
        QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            QString chatName = obj["chatName"].toString();
            // Ищем chatId по контексту — обновляем имя если оно отличается
            // Этот ответ приходит когда мы запрашиваем CHAT_INFO для конкретного чата
            // Обновим имя текущего чата если оно изменилось
            if (!chatName.isEmpty()) {
                for (int i = 0; i < ui->comboBox_chats->count(); ++i) {
                    int cid = ui->comboBox_chats->itemData(i).toInt();
                    if (cid == m_currentChatId) {
                        if (m_chats[cid] != chatName && cid != 1) {
                            m_chats[cid] = chatName;
                            ui->comboBox_chats->setItemText(i, chatName);
                            updateChatTitle();
                        }
                        break;
                    }
                }
            }
        }
        return;
    }

    if (raw.startsWith("OK|USER_INFO|")) {
        QString jsonData = raw.mid(13);
        QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            QJsonArray chatsArray = obj["chats"].toArray();

            // Собираем новый список чатов
            QMap<int, QString> newChats;
            newChats[1] = "Global Chat";

            for (const QJsonValue &val : chatsArray) {
                int chatId = val.toInt();
                if (chatId == 1) continue;
                // Сохраняем старое имя если было, иначе дефолтное
                if (m_chats.contains(chatId)) {
                    newChats[chatId] = m_chats[chatId];
                } else {
                    newChats[chatId] = "Чат " + QString::number(chatId);
                }
            }

            // Проверяем изменился ли список
            bool chatsChanged = (newChats.keys() != m_chats.keys());

            if (chatsChanged) {
                m_chats = newChats;
                m_updatingChats = true;
                ui->comboBox_chats->clear();

                for (auto it = m_chats.begin(); it != m_chats.end(); ++it) {
                    ui->comboBox_chats->addItem(it.value(), it.key());
                }

                m_updatingChats = false;

                int idx = ui->comboBox_chats->findData(m_currentChatId);
                if (idx != -1) {
                    ui->comboBox_chats->setCurrentIndex(idx);
                } else {
                    m_currentChatId = 1;
                    ui->comboBox_chats->setCurrentIndex(0);
                }
            }

            // Запрашиваем актуальные имена чатов с сервера
            for (auto it = m_chats.begin(); it != m_chats.end(); ++it) {
                if (it.key() != 1) {
                    ClientManager::getInstance()->sendSystemMessage(
                        QString("CHAT_INFO|%1").arg(it.key()));
                }
            }

            // Если это первая загрузка (чат пустой), загружаем историю
            if (ui->listWidget_chat->count() == 0) {
                ClientManager::getInstance()->sendSystemMessage(
                    QString("HISTORY|%1").arg(m_currentChatId));
            }

            updateChatTitle();
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
            // Игнорируем
        } else if (errorType == "NOT_IN_CHAT") {
            // Игнорируем — может прийти при обновлении чатов
        } else {
            addMessage("⚠️ Система", "Ошибка: " + errorType, false);
        }
        return;
    }

    if (raw.startsWith("OK|")) {
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
    if (m_updatingChats) return;
    if (index < 0) return;

    int chatId = ui->comboBox_chats->itemData(index).toInt();
    if (chatId == m_currentChatId) return;

    m_currentChatId = chatId;
    ui->listWidget_chat->clear();
    updateChatTitle();
    ClientManager::getInstance()->sendSystemMessage(QString("HISTORY|%1").arg(chatId));
}

void MainWindow::onManageChatClicked()
{
    QString chatName = m_chats.value(m_currentChatId, "Чат");
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
        "Вы уверены, что хотите покинуть чат \"" + m_chats.value(m_currentChatId, "Чат") + "\"?",
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
                                            m_chats.value(m_currentChatId, ""),
                                            &ok);
    if (ok && !newName.isEmpty()) {
        m_pendingRenameName = newName;
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
