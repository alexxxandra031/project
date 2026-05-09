#include "adminwindow.h"
#include "ui_adminwindow.h"
#include "clientmanager.h"
#include <QTableWidgetItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHeaderView> // Обязательно добавляем для настройки колонок

AdminWindow::AdminWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);

    ui->tableWidget_stats->setColumnCount(2);
    ui->tableWidget_stats->setHorizontalHeaderLabels({"Метрика", "Значение"});

    ui->tableWidget_stats->verticalHeader()->setVisible(false);
    ui->tableWidget_stats->verticalHeader()->setDefaultSectionSize(45);

    ui->tableWidget_stats->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->tableWidget_stats->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignCenter);

    ui->tableWidget_stats->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget_stats->horizontalHeader()->setStretchLastSection(true);

    ui->tableWidget_stats->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ClientManager::getInstance(),
            &ClientManager::dataReceived,
            this,
            &AdminWindow::onDataReceived);

    on_pushButton_refresh_clicked();
}

AdminWindow::~AdminWindow()
{
    delete ui;
}

void AdminWindow::onDataReceived(const QByteArray &data) {
    QString str = QString::fromUtf8(data);

    if (!str.startsWith("OK|STAT|")) return;

    QString jsonData = str.mid(8);
    QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        int usersCount = obj["users"].toInt();
        int messagesCount = obj["messages"].toInt();

        ui->tableWidget_stats->setRowCount(0);

        // --- Первая строка ---
        ui->tableWidget_stats->insertRow(0);
        QTableWidgetItem *itemMetric1 = new QTableWidgetItem("Всего пользователей");
        QTableWidgetItem *itemVal1 = new QTableWidgetItem(QString::number(usersCount));

        // Обязательно центрируем значение, чтобы оно встало ровно под заголовком
        itemVal1->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget_stats->setItem(0, 0, itemMetric1);
        ui->tableWidget_stats->setItem(0, 1, itemVal1);

        // --- Вторая строка ---
        ui->tableWidget_stats->insertRow(1);
        QTableWidgetItem *itemMetric2 = new QTableWidgetItem("Всего сообщений");
        QTableWidgetItem *itemVal2 = new QTableWidgetItem(QString::number(messagesCount));

        // Центрируем значение
        itemVal2->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget_stats->setItem(1, 0, itemMetric2);
        ui->tableWidget_stats->setItem(1, 1, itemVal2);
    }
}

void AdminWindow::on_pushButton_refresh_clicked() {
    // Отправляем существующую на сервере команду
    ClientManager::getInstance()->sendSystemMessage("STAT");
}
