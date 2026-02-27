#include "adminwindow.h"
#include "ui_adminwindow.h"
#include "clientmanager.h"
#include <QTableWidgetItem>

AdminWindow::AdminWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);

    ui->tableWidget_stats->setColumnCount(3);
    ui->tableWidget_stats->setHorizontalHeaderLabels({"Логин", "Статус", "Сообщений"});

    ui->tableWidget_stats->horizontalHeader()->setStretchLastSection(true);

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
    if (!data.startsWith("STATS|")) return;

    QString rawData = QString::fromUtf8(data).mid(6);

    ui->tableWidget_stats->setRowCount(0);

    QStringList users = rawData.split(';');

    for (const QString &userRow : users) {
        if (userRow.isEmpty()) continue;

        QStringList details = userRow.split(':');

        if (details.size() == 3) {
            int row = ui->tableWidget_stats->rowCount();
            ui->tableWidget_stats->insertRow(row);

            ui->tableWidget_stats->setItem(row, 0, new QTableWidgetItem(details[0]));
            ui->tableWidget_stats->setItem(row, 1, new QTableWidgetItem(details[1]));
            ui->tableWidget_stats->setItem(row, 2, new QTableWidgetItem(details[2]));
        }
    }
}

void AdminWindow::on_pushButton_refresh_clicked() {
    ClientManager::getInstance()->sendSystemMessage("GET_STATS");
}
