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
    m_showAllButton = new QPushButton("Показать всех", this);
    m_resultsList = new QListWidget(this);
    m_closeButton = new QPushButton("Закрыть", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_searchEdit);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_searchButton);
    btnLayout->addWidget(m_showAllButton);
    layout->addLayout(btnLayout);

    layout->addWidget(m_resultsList);
    layout->addWidget(m_closeButton);

    connect(m_searchButton, &QPushButton::clicked, this, &FindUserDialog::onSearchClicked);
    connect(m_showAllButton, &QPushButton::clicked, this, &FindUserDialog::onShowAllClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ClientManager::getInstance(), &ClientManager::dataReceived,
            this, &FindUserDialog::onDataReceived);

    connect(m_resultsList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        QString username = item->data(Qt::UserRole).toString();
        ClientManager::getInstance()->sendSystemMessage(
            QString("ADD_USER|%1|%2").arg(m_chatId).arg(username));
        QMessageBox::information(this, "Успех", "Запрос на добавление отправлен");
    });

    // Загружаем всех при открытии — используем пробел как wildcard
    onShowAllClicked();
}

void FindUserDialog::onSearchClicked()
{
    QString query = m_searchEdit->text().trimmed();
    if (query.isEmpty()) {
        onShowAllClicked();
        return;
    }
    ClientManager::getInstance()->sendSystemMessage(QString("FIND_USERS|%1").arg(query));
}

void FindUserDialog::onShowAllClicked()
{
    // Отправляем символ подчёркивания как минимальный wildcard
    // Сервер делает LIKE '%_%' — найдёт всех с хотя бы 1 символом в имени
    ClientManager::getInstance()->sendSystemMessage("FIND_USERS|_");
}

void FindUserDialog::onDataReceived(const QByteArray &data)
{
    QString raw = QString::fromUtf8(data);

    if (raw.startsWith("OK|FIND_USERS|")) {
        QString csvData = raw.mid(14);
        QStringList usernames = csvData.split(',', Qt::SkipEmptyParts);

        m_resultsList->clear();
        QString currentUser = ClientManager::getInstance()->username();
        for (const QString &username : usernames) {
            QString trimmed = username.trimmed();
            if (trimmed.isEmpty()) continue;
            if (trimmed == currentUser) continue;
            QListWidgetItem *item = new QListWidgetItem(trimmed, m_resultsList);
            item->setData(Qt::UserRole, trimmed);
        }

        if (m_resultsList->count() == 0) {
            QMessageBox::information(this, "Результат", "Пользователи не найдены");
        }
    }
    else if (raw.startsWith("ERROR|USER_NOT_FOUND")) {
        m_resultsList->clear();
        QMessageBox::information(this, "Результат", "Пользователи не найдены");
    }
}
