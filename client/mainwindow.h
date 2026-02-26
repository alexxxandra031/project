#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clientmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setAdminRole(bool isAdmin);

private slots:
    void on_pushButton_send_clicked();

    void onConnected();
    void onDataReceived(const QByteArray &data);

    void on_pushButton_adminPanel_clicked();

private:
    Ui::MainWindow *ui;
    bool m_isAdmin;
};
#endif // MAINWINDOW_H
