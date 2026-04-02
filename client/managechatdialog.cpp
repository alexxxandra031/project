#include "managechatdialog.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMenu>
#include <QAction>
void ManageChatDialog::onFindUserClicked()
{
    FindUserDialog dialog(m_chatId, this);
    dialog.exec();
    loadParticipants(); // обновим список участников на случай добавления
}

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

    m_participantsList->setContextMenuPolicy(Qt::CustomContextMenu);
    m_findButton = new QPushButton("Поиск пользователей", this);

    btnLayout->addWidget(m_findButton);

    connect(m_findButton, &QPushButton::clicked,
            this, &ManageChatDialog::onFindUserClicked);
    connect(m_participantsList, &QListWidget::customContextMenuRequested,
            this, &ManageChatDialog::showContextMenu);
    connect(m_addButton, &QPushButton::clicked,
            this, &ManageChatDialog::onAddUserClicked);
    connect(m_closeButton, &QPushButton::clicked,
            this, &QDialog::accept);
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
    else if (raw.startsWith("OK|REMOVE_USER")) {
        QMessageBox::information(this, "Успех", "Пользователь удалён из чата");
        loadParticipants();
    }
    else if (raw.startsWith("ERROR|")) {
        QString error = raw.mid(6);
        if (error == "NOT_CREATOR")
            QMessageBox::warning(this, "Ошибка", "Только создатель чата может удалять пользователей");
        else if (error == "CANNOT_REMOVE_CREATOR")
            QMessageBox::warning(this, "Ошибка", "Нельзя удалить создателя чата");
        else if (error == "USER_NOT_FOUND")
            QMessageBox::warning(this, "Ошибка", "Пользователь не найден");
        else
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить пользователя");
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

void ManageChatDialog::showContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = m_participantsList->itemAt(pos);
    if (!item) return;
    QString itemText = item->text();
    QString username = itemText.split('(').first().trimmed();
    QString currentUser = ClientManager::getInstance()->username();
    if (username == currentUser) return;

    QMenu menu(this);
    QAction *removeAction = menu.addAction("Удалить пользователя");
    connect(removeAction, &QAction::triggered, [this, username]() { onRemoveUser(username); });
    menu.exec(m_participantsList->mapToGlobal(pos));
}

void ManageChatDialog::onRemoveUser(const QString &username)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Удаление пользователя",
        QString("Удалить пользователя %1 из чата?").arg(username),
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ClientManager::getInstance()->sendSystemMessage(QString("REMOVE_USER|%1|%2").arg(m_chatId).arg(username));
    }
}

void ManageChatDialog::onFindUserClicked()
{
    FindUserDialog dialog(m_chatId, this);
    dialog.exec();
    loadParticipants();
}
