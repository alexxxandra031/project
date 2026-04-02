#include "managechatdialog.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ManageChatDialog::ManageChatDialog(int chatId, const QString &chatName, QWidget *parent)
    : QDialog(parent), m_chatId(chatId), m_chatName(chatName)
{
    setWindowTitle("Участники чата: " + chatName);
    setMinimumSize(300, 400);

    m_participantsList = new QListWidget(this);
    m_userNameEdit = new QLineEdit(this);
    m_userNameEdit->setPlaceholderText("Имя пользователя для добавления");
    m_addButton = new QPushButton("Добавить", this);
    m_closeButton = new QPushButton("Закрыть", this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_participantsList);
    mainLayout->addWidget(m_userNameEdit);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_addButton);
    btnLayout->addWidget(m_closeButton);
    mainLayout->addLayout(btnLayout);

    connect(m_addButton, &QPushButton::clicked, this, &ManageChatDialog::onAddUserClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ClientManager::getInstance(), &ClientManager::dataReceived,
            this, &ManageChatDialog::onDataReceived);

    loadParticipants();
}

void ManageChatDialog::loadParticipants()
{
    ClientManager::getInstance()->sendSystemMessage(QString("CHAT_INFO|%1").arg(m_chatId));
}

void ManageChatDialog::onDataReceived(const QByteArray &data)
{
    QString raw = QString::fromUtf8(data);
    if (raw.startsWith("OK|CHAT_INFO|")) {
        QString jsonData = raw.mid(13);
        QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            QJsonArray participants = obj["participants"].toArray();
            m_participantsList->clear();
            for (const QJsonValue &val : participants) {
                QJsonObject p = val.toObject();
                QString username = p["username"].toString();
                QString status = p["status"].toString();
                m_participantsList->addItem(QString("%1 (%2)").arg(username, status));
            }
        }
    }
    else if (raw.startsWith("OK|ADD_USER")) {
        QMessageBox::information(this, "Успех", "Пользователь добавлен в чат");
        loadParticipants(); // обновляем список
        m_userNameEdit->clear();
    }
    else if (raw.startsWith("ERROR|")) {
        QString error = raw.mid(6);
        if (error == "USER_NOT_FOUND")
            QMessageBox::warning(this, "Ошибка", "Пользователь не найден");
        else if (error == "USER_ALREADY_IN_CHAT")
            QMessageBox::warning(this, "Ошибка", "Пользователь уже в чате");
        else
            QMessageBox::warning(this, "Ошибка", "Не удалось добавить пользователя");
    }
}

void ManageChatDialog::onAddUserClicked()
{
    QString username = m_userNameEdit->text().trimmed();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Внимание", "Введите имя пользователя");
        return;
    }
    ClientManager::getInstance()->sendSystemMessage(QString("ADD_USER|%1|%2").arg(m_chatId).arg(username));
}
