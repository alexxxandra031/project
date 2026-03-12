#include "clienthandler.h"
#include "chatservice.h"

ClientHandler::ClientHandler(QTcpSocket* sock, QObject* parent, ChatService* chatSvc)
    : QObject(parent), socket(sock), m_chatService(chatSvc)
{
}



QString ClientHandler::parseRequest(const QString& request) {
    QStringList parts = request.split(";");


    if(parts.size() == 0)
        return "ERROR";

    QString command = parts[0];

    if(command == "LOGIN" && parts.size() == 3)
    {
        return loginUser(parts[1], parts[2]);
    }

    if(command == "REGISTER" && parts.size() == 3)
    {
        return registerUser(parts[1], parts[2]);
    }

    if(command == "SEND" && parts.size() == 4)
    {
        return handleSendMessage(parts[1], parts[2], parts[3]);
    }

    if(command == "STATUS" && parts.size() == 3)
    {
        return setUserStatus(parts[1], parts[2]);
    }

    if(command == "GET_HISTORY" && parts.size() == 3)
    {
        return getChatHistory(parts[1], parts[2]);
    }

    return "Неопознанная команда";
}

//заглушка для метода входа пользователя
QString ClientHandler::loginUser(const QString& login, const QString& password) {
    qDebug() << "Попытка входа:" << login << password;
    return "Вход выполнен успешно";
}

//заглушка для метода регистрации пользователя
QString ClientHandler::registerUser(const QString& login, const QString& password) {
    qDebug() << "Попытка регистрации:" << login << password;
    return "Регистрация выполнена успешно";
}

//заглушка для метода отправки сообщения
QString ClientHandler::handleSendMessage(const QString& sender, const QString& receiver, const QString& message) {
    qDebug() << "Обработка отправки сообщения: "<< message << "от" << sender << "к" << receiver;

    bool savingMessage = m_chatService->saveMessage(sender, receiver, message);

    if(!savingMessage) return "Ошибка сохранения сообщения";

    m_chatService->deliverMessage(sender, receiver, message);

    return "Сообщение принято сервером";
}


//заглушка для метода обозначения статуса пользователя(онлайн/оффлайн)
QString ClientHandler::setUserStatus(const QString& user, const QString& status) {
    qDebug() << "Установка статуса:" << user << status;
    return "Статус пользователя был успешно сменен";
}


QString ClientHandler::getChatHistory(const QString& user1, const QString& user2)
{
    qDebug() << "Запрос истории чата между:" << user1 << "и" << user2;

    //пока заглушка, позже будет осуществляться доступ к бд
    return "HISTORY;Сообщение1;Сообщение2?;Сообщение3";
}
