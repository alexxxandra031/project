/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_top;
    QLabel *label_chatTitle;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_adminPanel;
    QHBoxLayout *horizontalLayout_chatSelect;
    QComboBox *comboBox_chats;
    QPushButton *pushButton_createChat;
    QPushButton *pushButton_manageChat;
    QPushButton *pushButton_renameChat;
    QPushButton *pushButton_leaveChat;
    QListWidget *listWidget_chat;
    QHBoxLayout *horizontalLayout_bottom;
    QLineEdit *lineEdit_message;
    QPushButton *pushButton_send;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(812, 600);
        MainWindow->setStyleSheet(QString::fromUtf8("\n"
"    /* \320\223\320\273\320\276\320\261\320\260\320\273\321\214\320\275\321\213\320\271 \321\204\320\276\320\275 \320\276\320\272\320\275\320\260 */\n"
"    QMainWindow {\n"
"        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\n"
"                                    stop:0 #667eea, stop:1 #764ba2);\n"
"    }\n"
"    /* \320\246\320\265\320\275\321\202\321\200\320\260\320\273\321\214\320\275\321\213\320\271 \320\262\320\270\320\264\320\266\320\265\321\202 (\320\277\321\200\320\276\320\267\321\200\320\260\321\207\320\275\321\213\320\271, \321\207\321\202\320\276\320\261\321\213 \321\204\320\276\320\275 \320\261\321\213\320\273 \320\262\320\270\320\264\320\265\320\275) */\n"
"    QWidget#centralwidget {\n"
"        background: transparent;\n"
"    }\n"
"    /* \320\237\320\276\320\273\320\276\321\201\321\213 \320\277\321\200\320\276\320\272\321\200\321\203\321\202\320\272\320\270 (\321\201\321\202\320\270\320\273\320\270\320\267\320\260\321\206\320\270\321\217) */\n"
"    QScrollBar:vertical {\n"
"  "
                        "      border: none;\n"
"        background: rgba(255,255,255,0.2);\n"
"        width: 8px;\n"
"        border-radius: 4px;\n"
"    }\n"
"    QScrollBar::handle:vertical {\n"
"        background: rgba(255,255,255,0.5);\n"
"        border-radius: 4px;\n"
"    }\n"
"    QScrollBar::handle:vertical:hover {\n"
"        background: rgba(255,255,255,0.7);\n"
"    }\n"
"    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"        border: none;\n"
"        background: none;\n"
"    }\n"
"   "));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setSpacing(15);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(20, 20, 20, 20);
        horizontalLayout_top = new QHBoxLayout();
        horizontalLayout_top->setObjectName("horizontalLayout_top");
        label_chatTitle = new QLabel(centralwidget);
        label_chatTitle->setObjectName("label_chatTitle");
        label_chatTitle->setStyleSheet(QString::fromUtf8("QLabel { color: white; }"));

        horizontalLayout_top->addWidget(label_chatTitle);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_top->addItem(horizontalSpacer);

        pushButton_adminPanel = new QPushButton(centralwidget);
        pushButton_adminPanel->setObjectName("pushButton_adminPanel");
        pushButton_adminPanel->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pushButton_adminPanel->setStyleSheet(QString::fromUtf8("\n"
"          QPushButton {\n"
"              background-color: rgba(255, 255, 255, 0.15);\n"
"              border: 1px solid rgba(255, 255, 255, 0.4);\n"
"              border-radius: 18px;\n"
"              padding: 8px 20px;\n"
"              color: white;\n"
"              font-weight: bold;\n"
"              font-size: 13px;\n"
"          }\n"
"          QPushButton:hover {\n"
"              background-color: rgba(255, 255, 255, 0.25);\n"
"              border: 1px solid rgba(255, 255, 255, 0.7);\n"
"          }\n"
"          QPushButton:pressed {\n"
"              background-color: rgba(255, 255, 255, 0.1);\n"
"              border: 1px solid rgba(255, 255, 255, 0.3);\n"
"          }\n"
"         "));

        horizontalLayout_top->addWidget(pushButton_adminPanel);


        verticalLayout->addLayout(horizontalLayout_top);

        horizontalLayout_chatSelect = new QHBoxLayout();
        horizontalLayout_chatSelect->setSpacing(10);
        horizontalLayout_chatSelect->setObjectName("horizontalLayout_chatSelect");
        comboBox_chats = new QComboBox(centralwidget);
        comboBox_chats->setObjectName("comboBox_chats");
        comboBox_chats->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        comboBox_chats->setStyleSheet(QString::fromUtf8("\n"
"          QComboBox {\n"
"              background-color: rgba(255, 255, 255, 0.15);\n"
"              border: 1px solid rgba(255, 255, 255, 0.4);\n"
"              border-radius: 18px;\n"
"              padding: 8px 15px;\n"
"              color: white;\n"
"              font-weight: bold;\n"
"              font-size: 13px;\n"
"          }\n"
"          QComboBox:hover {\n"
"              background-color: rgba(255, 255, 255, 0.25);\n"
"              border: 1px solid rgba(255, 255, 255, 0.7);\n"
"          }\n"
"          QComboBox::drop-down {\n"
"              border: none;\n"
"              width: 30px;\n"
"          }\n"
"          QComboBox::down-arrow {\n"
"              /* \320\222\321\201\321\202\321\200\320\276\320\265\320\275\320\275\320\260\321\217 \321\201\321\202\321\200\320\265\320\273\320\272\320\260 Qt \320\264\320\273\321\217 \320\263\320\273\320\260\321\201\321\201\320\274\320\276\321\200\321\204\320\270\320\267\320\274\320\260 */\n"
"              image: none;\n"
"          }\n"
"    "
                        "      QComboBox QAbstractItemView {\n"
"              background-color: #764ba2;\n"
"              color: white;\n"
"              selection-background-color: rgba(255, 255, 255, 0.3);\n"
"              selection-color: white;\n"
"              border: 1px solid rgba(255, 255, 255, 0.4);\n"
"              border-radius: 10px;\n"
"              outline: none;\n"
"              padding: 5px;\n"
"          }\n"
"         "));

        horizontalLayout_chatSelect->addWidget(comboBox_chats);

        pushButton_createChat = new QPushButton(centralwidget);
        pushButton_createChat->setObjectName("pushButton_createChat");
        pushButton_createChat->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pushButton_createChat->setStyleSheet(QString::fromUtf8("\n"
"          QPushButton {\n"
"              background-color: rgba(255, 255, 255, 0.15);\n"
"              border: 1px solid rgba(255, 255, 255, 0.4);\n"
"              border-radius: 18px;\n"
"              padding: 8px 15px;\n"
"              color: white;\n"
"              font-weight: bold;\n"
"              font-size: 13px;\n"
"          }\n"
"          QPushButton:hover {\n"
"              background-color: rgba(255, 255, 255, 0.25);\n"
"              border: 1px solid rgba(255, 255, 255, 0.7);\n"
"          }\n"
"          QPushButton:pressed {\n"
"              background-color: rgba(255, 255, 255, 0.1);\n"
"              border: 1px solid rgba(255, 255, 255, 0.3);\n"
"          }\n"
"         "));

        horizontalLayout_chatSelect->addWidget(pushButton_createChat);

        pushButton_manageChat = new QPushButton(centralwidget);
        pushButton_manageChat->setObjectName("pushButton_manageChat");
        pushButton_manageChat->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pushButton_manageChat->setStyleSheet(QString::fromUtf8("\n"
"          QPushButton {\n"
"              background-color: rgba(255, 255, 255, 0.15);\n"
"              border: 1px solid rgba(255, 255, 255, 0.4);\n"
"              border-radius: 18px;\n"
"              padding: 8px 15px;\n"
"              color: white;\n"
"              font-weight: bold;\n"
"              font-size: 13px;\n"
"          }\n"
"          QPushButton:hover {\n"
"              background-color: rgba(255, 255, 255, 0.25);\n"
"              border: 1px solid rgba(255, 255, 255, 0.7);\n"
"          }\n"
"          QPushButton:pressed {\n"
"              background-color: rgba(255, 255, 255, 0.1);\n"
"              border: 1px solid rgba(255, 255, 255, 0.3);\n"
"          }\n"
"         "));

        horizontalLayout_chatSelect->addWidget(pushButton_manageChat);

        pushButton_renameChat = new QPushButton(centralwidget);
        pushButton_renameChat->setObjectName("pushButton_renameChat");
        pushButton_renameChat->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pushButton_renameChat->setStyleSheet(QString::fromUtf8("\n"
"          QPushButton {\n"
"              background-color: rgba(255, 255, 255, 0.15);\n"
"              border: 1px solid rgba(255, 255, 255, 0.4);\n"
"              border-radius: 18px;\n"
"              padding: 8px 15px;\n"
"              color: white;\n"
"              font-weight: bold;\n"
"              font-size: 13px;\n"
"          }\n"
"          QPushButton:hover {\n"
"              background-color: rgba(255, 255, 255, 0.25);\n"
"              border: 1px solid rgba(255, 255, 255, 0.7);\n"
"          }\n"
"          QPushButton:pressed {\n"
"              background-color: rgba(255, 255, 255, 0.1);\n"
"              border: 1px solid rgba(255, 255, 255, 0.3);\n"
"          }\n"
"         "));

        horizontalLayout_chatSelect->addWidget(pushButton_renameChat);

        pushButton_leaveChat = new QPushButton(centralwidget);
        pushButton_leaveChat->setObjectName("pushButton_leaveChat");
        pushButton_leaveChat->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pushButton_leaveChat->setStyleSheet(QString::fromUtf8("\n"
"          QPushButton {\n"
"              background-color: rgba(255, 100, 100, 0.25);\n"
"              border: 1px solid rgba(255, 100, 100, 0.5);\n"
"              border-radius: 18px;\n"
"              padding: 8px 15px;\n"
"              color: #ffcccc;\n"
"              font-weight: bold;\n"
"              font-size: 13px;\n"
"          }\n"
"          QPushButton:hover {\n"
"              background-color: rgba(255, 80, 80, 0.4);\n"
"              border: 1px solid rgba(255, 100, 100, 0.8);\n"
"              color: white;\n"
"          }\n"
"          QPushButton:pressed {\n"
"              background-color: rgba(255, 50, 50, 0.5);\n"
"              border: 1px solid rgba(255, 50, 50, 0.9);\n"
"          }\n"
"         "));

        horizontalLayout_chatSelect->addWidget(pushButton_leaveChat);


        verticalLayout->addLayout(horizontalLayout_chatSelect);

        listWidget_chat = new QListWidget(centralwidget);
        listWidget_chat->setObjectName("listWidget_chat");
        listWidget_chat->setStyleSheet(QString::fromUtf8("\n"
"           QListWidget {\n"
"               background-color: white;\n"
"               border: none;\n"
"               border-radius: 20px;\n"
"               padding: 10px;\n"
"               outline: none;\n"
"           }\n"
"           QListWidget::item {\n"
"               border: none;\n"
"               background-color: transparent;\n"
"           }\n"
"          "));
        listWidget_chat->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        listWidget_chat->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);

        verticalLayout->addWidget(listWidget_chat);

        horizontalLayout_bottom = new QHBoxLayout();
        horizontalLayout_bottom->setObjectName("horizontalLayout_bottom");
        lineEdit_message = new QLineEdit(centralwidget);
        lineEdit_message->setObjectName("lineEdit_message");
        lineEdit_message->setStyleSheet(QString::fromUtf8("\n"
"          QLineEdit {\n"
"              background-color: white;\n"
"              border: 2px solid transparent;\n"
"              border-radius: 25px;\n"
"              padding: 10px 20px;\n"
"              font-size: 14px;\n"
"              selection-background-color: #667eea;\n"
"          }\n"
"          QLineEdit:focus {\n"
"              border: 2px solid #667eea;\n"
"          }\n"
"         "));

        horizontalLayout_bottom->addWidget(lineEdit_message);

        pushButton_send = new QPushButton(centralwidget);
        pushButton_send->setObjectName("pushButton_send");
        QFont font;
        font.setPointSize(18);
        font.setBold(true);
        pushButton_send->setFont(font);
        pushButton_send->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pushButton_send->setStyleSheet(QString::fromUtf8("\n"
"          QPushButton {\n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                          stop:0 #667eea, stop:1 #764ba2);\n"
"              border: none;\n"
"              border-radius: 25px;\n"
"              min-width: 50px;\n"
"              min-height: 50px;\n"
"              color: white;\n"
"          }\n"
"          QPushButton:hover {\n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                          stop:0 #5a67d8, stop:1 #6b46a1);\n"
"          }\n"
"          QPushButton:pressed {\n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                          stop:0 #4c51bf, stop:1 #553c8b);\n"
"          }\n"
"         "));

        horizontalLayout_bottom->addWidget(pushButton_send);


        verticalLayout->addLayout(horizontalLayout_bottom);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 812, 21));
        menubar->setVisible(false);
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        statusbar->setVisible(false);
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\320\247\320\260\321\202", nullptr));
        label_chatTitle->setText(QCoreApplication::translate("MainWindow", "<h2>\360\237\222\254 \320\236\320\261\321\211\320\270\320\271 \321\207\320\260\321\202</h2>", nullptr));
        pushButton_adminPanel->setText(QCoreApplication::translate("MainWindow", "\342\232\231\357\270\217 \320\237\320\260\320\275\320\265\320\273\321\214 \321\203\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\321\217", nullptr));
        pushButton_createChat->setText(QCoreApplication::translate("MainWindow", "\342\236\225 \320\241\320\276\320\267\320\264\320\260\321\202\321\214 \321\207\320\260\321\202", nullptr));
        pushButton_manageChat->setText(QCoreApplication::translate("MainWindow", "\360\237\221\245 \320\243\321\207\320\260\321\201\321\202\320\275\320\270\320\272\320\270", nullptr));
        pushButton_renameChat->setText(QCoreApplication::translate("MainWindow", "\342\234\217\357\270\217 \320\237\320\265\321\200\320\265\320\270\320\274\320\265\320\275\320\276\320\262\320\260\321\202\321\214", nullptr));
        pushButton_leaveChat->setText(QCoreApplication::translate("MainWindow", "\360\237\232\252 \320\222\321\213\320\271\321\202\320\270 \320\270\320\267 \321\207\320\260\321\202\320\260", nullptr));
        lineEdit_message->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\222\320\262\320\265\320\264\320\270\321\202\320\265 \321\201\320\276\320\276\320\261\321\211\320\265\320\275\320\270\320\265...", nullptr));
        pushButton_send->setText(QCoreApplication::translate("MainWindow", "\342\236\244", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
