#include "adminwindow.h"
#include "ui_adminwindow.h"
#include <QTableWidgetItem>

AdminWindow::AdminWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);

    ui->tableWidget_stats->setColumnCount(3);
    ui->tableWidget_stats->setHorizontalHeaderLabels({"Логин", "Статус", "Сообщений"});

    ui->tableWidget_stats->horizontalHeader()->setStretchLastSection(true);

    loadMockData();
}

AdminWindow::~AdminWindow()
{
    delete ui;
}

void AdminWindow::loadMockData()
{
    ui->tableWidget_stats->setRowCount(0);

    // ЗАГЛУШКА
    // ClientManager::getInstance()->sendMessage("GET_STATS");
    // в теории сервером возвращается текст который распарсится на клиенте

    ui->tableWidget_stats->insertRow(0);
    ui->tableWidget_stats->setItem(0, 0, new QTableWidgetItem("admin"));
    ui->tableWidget_stats->setItem(0, 1, new QTableWidgetItem("Online"));
    ui->tableWidget_stats->setItem(0, 2, new QTableWidgetItem("15"));

    ui->tableWidget_stats->insertRow(1);
    ui->tableWidget_stats->setItem(1, 0, new QTableWidgetItem("user1"));
    ui->tableWidget_stats->setItem(1, 1, new QTableWidgetItem("Offline"));
    ui->tableWidget_stats->setItem(1, 2, new QTableWidgetItem("42"));

    ui->tableWidget_stats->insertRow(2);
    ui->tableWidget_stats->setItem(2, 0, new QTableWidgetItem("cyka"));
    ui->tableWidget_stats->setItem(2, 1, new QTableWidgetItem("Online"));
    ui->tableWidget_stats->setItem(2, 2, new QTableWidgetItem("666"));
}

void AdminWindow::on_pushButton_refresh_clicked() {

    loadMockData();
}
