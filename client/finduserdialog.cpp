#include "finduserdialog.h"
#include "clientmanager.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>

FindUserDialog::FindUserDialog(int chatId, QWidget *parent)
    : QDialog(parent), m_chatId(chatId)
{
    setWindowTitle("Поиск пользователей");
    setMinimumSize(300, 400);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Введите имя пользователя...");
    m_searchButton = new QPushButton("Найти", this);
    m_resultsList = new QListWidget(this);
    m_closeButton = new QPushButton("Закрыть", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_searchEdit);
    layout->addWidget(m_searchButton);
    layout->addWidget(m_resultsList);
    layout->addWidget(m_closeButton);

    connect(m_searchButton, &QPushButton::clicked, this, &FindUserDialog::onSearchClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ClientManager::getInstance(), &ClientManager::dataReceived,
            this, &FindUserDialog::onDataReceived);
}

void FindUserDialog::onSearchClicked()
{
    QString query = m_searchEdit->text().trimmed();
    if (query.isEmpty()) {
        QMessageBox::warning(this, "Внимание", "Введите поисковый запрос");
        return;
    }
    ClientManager::getInstance()->sendSystemMessage(QString("FIND_USERS|%1").arg(query));
}

void FindUserDialog::onDataReceived(const QByteArray &data)
{
    QString raw = QString::fromUtf8(data);
    if (raw.startsWith("OK|FIND_USERS|")) {
        QString jsonData = raw.mid(13);
        QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            m_resultsList->clear();
            for (const QJsonValue &val : arr) {
                QString username = val.toString();
                QListWidgetItem *item = new QListWidgetItem(username, m_resultsList);
                item->setData(Qt::UserRole, username);
            }
        }
    }
    else if (raw.startsWith("ERROR|")) {
        QString error = raw.mid(6);
        if (error == "USER_NOT_FOUND")
            QMessageBox::information(this, "Результат", "Пользователи не найдены");
        else
            QMessageBox::warning(this, "Ошибка", "Ошибка поиска: " + error);
    }
}

void FindUserDialog::onAddUser(const QString &username)
{
    ClientManager::getInstance()->sendSystemMessage(QString("ADD_USER|%1|%2").arg(m_chatId).arg(username));
    QMessageBox::information(this, "Успех", "Запрос на добавление отправлен");
    // Можно закрыть диалог или очистить список
    accept();
}
