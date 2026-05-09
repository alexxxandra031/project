#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QString>
#include <QSettings>

class ThemeManager
{
public:
    // Загрузить сохранённую тему из настроек (вызвать в main.cpp до создания окон)
    static void loadSettings();
    // Узнать, включена ли тёмная тема
    static bool isDark();
    // Установить тему и сохранить в настройках
    static void setDark(bool dark);

    // Готовые наборы стилей для разных окон
    static QString globalStyleSheet();        // общие стили (скроллбары, тултипы)
    static QString mainWindowStyle();         // MainWindow
    static QString authWindowStyle();         // AuthWindow
    static QString adminWindowStyle();        // AdminWindow
    static QString bubbleStyle(bool outgoing, bool dark); // пузырьки сообщений (используется в MainWindow::addMessage)
    static QString sendButtonStyle();    // <-- добавить
    static QString inputFieldStyle();    // <-- добавить

private:
    static bool m_dark;
    static QString lightGlobal();
    static QString darkGlobal();
    static QString lightMain();
    static QString darkMain();
    static QString lightAuth();
    static QString darkAuth();
    static QString lightAdmin();
    static QString darkAdmin();
};

#endif // THEMEMANAGER_H
