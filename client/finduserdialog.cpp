#include "finduserdialog.h"
#include "clientmanager.h"
#include <QMessageBox>

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

    connect(m_resultsList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        QString username = item->data(Qt::UserRole).toString();
        ClientManager::getInstance()->sendSystemMessage(
            QString("ADD_USER|%1|%2").arg(m_chatId).arg(username));
        QMessageBox::information(this, "Успех", "Запрос на добавление отправлен");
        accept();
    });
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

    // Сервер возвращает: OK|FIND_USERS|user1,user2,user3
    if (raw.startsWith("OK|FIND_USERS|")) {
        QString csvData = raw.mid(14);  // "OK|FIND_USERS|" = 14 символов
        QStringList usernames = csvData.split(',', Qt::SkipEmptyParts);

        m_resultsList->clear();
        for (const QString &username : usernames) {
            QString trimmed = username.trimmed();
            if (trimmed.isEmpty()) continue;
            QListWidgetItem *item = new QListWidgetItem(trimmed, m_resultsList);
            item->setData(Qt::UserRole, trimmed);
        }

        if (usernames.isEmpty()) {
            QMessageBox::information(this, "Результат", "Пользователи не найдены");
        }
    }
    else if (raw.startsWith("ERROR|USER_NOT_FOUND")) {
        QMessageBox::information(this, "Результат", "Пользователи не найдены");
    }
    else if (raw.startsWith("ERROR|")) {
        // Игнорируем ошибки, не относящиеся к поиску
        // (чтобы не показывать ошибки от других операций)
    }
}
