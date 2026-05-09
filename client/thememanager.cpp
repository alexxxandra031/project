#include "thememanager.h"

bool ThemeManager::m_dark = false;

void ThemeManager::loadSettings()
{
    QSettings settings("MessengerTeam", "Messenger");
    m_dark = settings.value("darkTheme", false).toBool();
}

bool ThemeManager::isDark()
{
    return m_dark;
}

void ThemeManager::setDark(bool dark)
{
    m_dark = dark;
    QSettings settings("MessengerTeam", "Messenger");
    settings.setValue("darkTheme", dark);
}

QString ThemeManager::globalStyleSheet()
{
    return isDark() ? darkGlobal() : lightGlobal();
}

QString ThemeManager::mainWindowStyle()
{
    return isDark() ? darkMain() : lightMain();
}

QString ThemeManager::authWindowStyle()
{
    return isDark() ? darkAuth() : lightAuth();
}

QString ThemeManager::adminWindowStyle()
{
    return isDark() ? darkAdmin() : lightAdmin();
}

QString ThemeManager::bubbleStyle(bool outgoing, bool dark)
{
    if (outgoing) {
        return "QFrame {"
               "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
               "                               stop:0 #667eea, stop:1 #764ba2);"
               "   border-radius: 12px;"
               "   padding: 6px 10px;"
               "}";
    } else {
        QString bg = dark ? "#2c2c2c" : "#f1f3f5";
        return "QFrame {"
               "   background-color: " + bg + ";"
                      "   border-radius: 12px;"
                      "   padding: 6px 10px;"
                      "}";
    }
}

QString ThemeManager::sendButtonStyle()
{
    if (isDark()) {
        return
            "QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "                               stop:0 #4a3a80, stop:1 #5b3a7a);"
            "   border: none;"
            "   border-radius: 25px;"
            "   min-width: 50px;"
            "   min-height: 50px;"
            "   color: white;"
            "   font-size: 18px;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "                               stop:0 #5e4d9e, stop:1 #6e4d8e);"
            "}"
            "QPushButton:pressed {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "                               stop:0 #3b2a6a, stop:1 #4b2a6a);"
            "}";
    } else {
        return
            "QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "                               stop:0 #667eea, stop:1 #764ba2);"
            "   border: none;"
            "   border-radius: 25px;"
            "   min-width: 50px;"
            "   min-height: 50px;"
            "   color: white;"
            "   font-size: 18px;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "                               stop:0 #5a67d8, stop:1 #6b46a1);"
            "}"
            "QPushButton:pressed {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "                               stop:0 #4c51bf, stop:1 #553c8b);"
            "}";
    }
}

QString ThemeManager::inputFieldStyle()
{
    if (isDark()) {
        // Светлое поле с тёмным текстом, как вы и хотели
        return
            "QLineEdit {"
            "   background-color: white;"
            "   color: black;"
            "   border: 2px solid #667eea;"
            "   border-radius: 25px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   selection-background-color: #667eea;"
            "}"
            "QLineEdit:focus {"
            "   border: 2px solid #764ba2;"
            "}";
    } else {
        return
            "QLineEdit {"
            "   background-color: white;"
            "   color: black;"
            "   border: 2px solid transparent;"
            "   border-radius: 25px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   selection-background-color: #667eea;"
            "}"
            "QLineEdit:focus {"
            "   border: 2px solid #667eea;"
            "}";
    }
}

// --------------- Light styles (оригинальные) ---------------
QString ThemeManager::lightGlobal()
{
    return
        "QToolTip { background-color: #333; color: #fff; border: 1px solid #667eea; }"
        "QScrollBar:vertical { background: transparent; width: 8px; }"
        "QScrollBar::handle:vertical { background: rgba(255,255,255,0.5); border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: rgba(255,255,255,0.7); }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { border: none; background: none; }";
}

QString ThemeManager::darkGlobal()
{
    return
        "QToolTip { background-color: #eee; color: #111; border: 1px solid #888; }"
        "QScrollBar:vertical { background: rgba(255,255,255,0.05); width: 8px; }"
        "QScrollBar::handle:vertical { background: rgba(255,255,255,0.2); border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: rgba(255,255,255,0.4); }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { border: none; background: none; }";
}

QString ThemeManager::lightMain()
{
    return
        "QMainWindow {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                               stop:0 #667eea, stop:1 #764ba2);"
        "}"
        "QWidget#centralwidget { background: transparent; }"
        "QLabel { color: white; }"
        // ComboBox
        "QComboBox {"
        "   background-color: rgba(255, 255, 255, 0.15);"
        "   border: 1px solid rgba(255, 255, 255, 0.4);"
        "   border-radius: 18px;"
        "   padding: 8px 15px;"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QComboBox:hover {"
        "   background-color: rgba(255, 255, 255, 0.25);"
        "   border: 1px solid rgba(255, 255, 255, 0.7);"
        "}"
        "QComboBox::drop-down { border: none; width: 30px; }"
        "QComboBox QAbstractItemView {"
        "   background-color: #764ba2;"
        "   color: white;"
        "   selection-background-color: rgba(255, 255, 255, 0.3);"
        "   selection-color: white;"
        "   border: 1px solid rgba(255, 255, 255, 0.4);"
        "   border-radius: 10px;"
        "   outline: none;"
        "   padding: 5px;"
        "}"
        // LineEdit
        "QLineEdit {"
        "   background-color: white;"
        "   border: 2px solid transparent;"
        "   border-radius: 25px;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   selection-background-color: #667eea;"
        "}"
        "QLineEdit:focus { border: 2px solid #667eea; }"
        // QListWidget
        "QListWidget {"
        "   background-color: white;"
        "   border: none;"
        "   border-radius: 20px;"
        "   padding: 10px;"
        "   outline: none;"
        "}"
        "QListWidget::item { border: none; background-color: transparent; }"
        // Кнопки с прозрачным фоном (общие для всех pushbutton в mainwindow.ui)
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 0.15);"
        "   border: 1px solid rgba(255, 255, 255, 0.4);"
        "   border-radius: 18px;"
        "   padding: 8px 15px;"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 0.25);"
        "   border: 1px solid rgba(255, 255, 255, 0.7);"
        "}"
        "QPushButton:pressed {"
        "   background-color: rgba(255, 255, 255, 0.1);"
        "   border: 1px solid rgba(255, 255, 255, 0.3);"
        "}"
        // Кнопка "Выйти из чата" (красноватая) переопределим отдельно, она задана в UI inline – оставим как есть, она будет белой на тёмном фоне? Лучше адаптировать. Но можно не менять.
        ;
}

QString ThemeManager::darkMain()
{
    return
        "QMainWindow {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                               stop:0 #2a2350, stop:1 #1a1330);"
        "}"
        "QWidget#centralwidget { background: transparent; }"
        "QLabel { color: #cccccc; }"
        "QComboBox {"
        "   background-color: rgba(255, 255, 255, 0.08);"
        "   border: 1px solid rgba(255, 255, 255, 0.2);"
        "   border-radius: 18px;"
        "   padding: 8px 15px;"
        "   color: #cccccc;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QComboBox:hover {"
        "   background-color: rgba(255, 255, 255, 0.15);"
        "   border: 1px solid rgba(255, 255, 255, 0.4);"
        "}"
        "QComboBox::drop-down { border: none; width: 30px; }"
        "QComboBox QAbstractItemView {"
        "   background-color: #2d2d2d;"
        "   color: #ccc;"
        "   selection-background-color: #444;"
        "   selection-color: white;"
        "   border: 1px solid #444;"
        "   border-radius: 10px;"
        "   outline: none;"
        "   padding: 5px;"
        "}"
        "QLineEdit {"
        "   background-color: #333;"
        "   color: #fff;"
        "   border: 2px solid #555;"
        "   border-radius: 25px;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   selection-background-color: #667eea;"
        "}"
        "QLineEdit:focus { border: 2px solid #888; }"
        "QListWidget {"
        "   background-color: #1e1e1e;"
        "   color: #ccc;"
        "   border: none;"
        "   border-radius: 20px;"
        "   padding: 10px;"
        "   outline: none;"
        "}"
        "QListWidget::item { border: none; background-color: transparent; }"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #4a3a80, stop:1 #5b3a7a);"
        "   border: none;"
        "   border-radius: 18px;"
        "   padding: 8px 15px;"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #5e4d9e, stop:1 #6e4d8e);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #3b2a6a, stop:1 #4b2a6a);"
        "}";
}

QString ThemeManager::lightAuth()
{
    return
        "QDialog {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                               stop:0 #667eea, stop:1 #764ba2);"
        "}"
        "QFrame#frame_card {"
        "   background-color: white;"
        "   border-radius: 20px;"
        "}"
        "QLabel { color: #333; }"
        "QLineEdit {"
        "   border: 1px solid #ddd;"
        "   border-radius: 10px;"
        "   padding: 0 10px;"
        "   background-color: #f9f9f9;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #667eea;"
        "   background-color: white;"
        "}"
        "QPushButton {"
        "   border: none;"
        "   border-radius: 25px;"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#pushButton_login {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #667eea, stop:1 #764ba2);"
        "}"
        "QPushButton#pushButton_login:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #5a67d8, stop:1 #6b46a1);"
        "}"
        "QPushButton#pushButton_login:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #4c51bf, stop:1 #553c8b);"
        "}"
        "QPushButton#pushButton_register {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #9f7aea, stop:1 #6b46c1);"
        "}"
        "QPushButton#pushButton_register:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #b794f4, stop:1 #805ad5);"
        "}"
        "QPushButton#pushButton_register:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #8b5cf6, stop:1 #5b21b6);"
        "}"
        "QLabel#label_footer { color: #888; }";
}

QString ThemeManager::darkAuth()
{
    return
        "QDialog {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                               stop:0 #2a2350, stop:1 #1a1330);"
        "}"
        "QFrame#frame_card {"
        "   background-color: #2d2d2d;"
        "   border-radius: 20px;"
        "}"
        "QLabel { color: #ccc; }"
        "QLineEdit {"
        "   border: 1px solid #555;"
        "   border-radius: 10px;"
        "   padding: 0 10px;"
        "   background-color: #333;"
        "   color: #fff;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #888;"
        "   background-color: #3a3a3a;"
        "}"
        "QPushButton {"
        "   border: none;"
        "   border-radius: 25px;"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#pushButton_login {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #4a3a80, stop:1 #5b3a7a);"
        "}"
        "QPushButton#pushButton_login:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #5e4d9e, stop:1 #6e4d8e);"
        "}"
        "QPushButton#pushButton_register {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #5b3a7a, stop:1 #4a3a80);"
        "}"
        "QPushButton#pushButton_register:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #6e4d8e, stop:1 #5e4d9e);"
        "}"
        "QLabel#label_footer { color: #888; }";
}

QString ThemeManager::lightAdmin()
{
    return
        "QDialog {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                               stop:0 #667eea, stop:1 #764ba2);"
        "}"
        "QFrame#frame_card {"
        "   background-color: white;"
        "   border-radius: 20px;"
        "   padding: 10px;"
        "}"
        "QLabel { color: white; }"
        // Таблица
        "QTableWidget {"
        "   border: none;"
        "   gridline-color: #e2e8f0;"
        "   selection-background-color: #667eea;"
        "   selection-color: white;"
        "   font-size: 14px;"
        "   background-color: white;"
        "   alternate-background-color: #f8f9fa;"
        "}"
        "QHeaderView::section {"
        "   background-color: #f7fafc;"
        "   padding: 8px;"
        "   border: none;"
        "   border-bottom: 2px solid #667eea;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "   color: #2d3748;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #667eea, stop:1 #764ba2);"
        "   border: none;"
        "   border-radius: 25px;"
        "   padding: 10px 30px;"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #5a67d8, stop:1 #6b46a1);"
        "}";
}

QString ThemeManager::darkAdmin()
{
    return
        "QDialog {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                               stop:0 #2a2350, stop:1 #1a1330);"
        "}"
        "QFrame#frame_card {"
        "   background-color: #2d2d2d;"
        "   border-radius: 20px;"
        "   padding: 10px;"
        "}"
        "QLabel { color: #ccc; }"
        "QTableWidget {"
        "   border: none;"
        "   gridline-color: #444;"
        "   selection-background-color: #667eea;"
        "   selection-color: white;"
        "   font-size: 14px;"
        "   background-color: #1e1e1e;"
        "   alternate-background-color: #2c2c2c;"
        "   color: #ccc;"
        "}"
        "QHeaderView::section {"
        "   background-color: #333;"
        "   padding: 8px;"
        "   border: none;"
        "   border-bottom: 2px solid #667eea;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "   color: #ccc;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #4a3a80, stop:1 #5b3a7a);"
        "   border: none;"
        "   border-radius: 25px;"
        "   padding: 10px 30px;"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #5e4d9e, stop:1 #6e4d8e);"
        "}";
}
