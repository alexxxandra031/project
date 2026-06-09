#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QDialog>

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();

private slots:
    void on_pushButton_refresh_clicked();
    void onDataReceived(const QByteArray &data);

private:
    Ui::AdminWindow *ui;
};

#endif // ADMINWINDOW_H
