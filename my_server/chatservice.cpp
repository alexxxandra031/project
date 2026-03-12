#include "chatservice.h"
#include <QDebug>

ChatService::ChatService()
{
    qDebug() << "ChatService создан";
}

//заглушка для метода отправки сообщения отправителю
void ChatService::deliverMessage(const QString& sender, const QString& receiver, const QString& message) {
    qDebug() << "Попытка доставки сообщения: "<< message << "от" << sender << "к" << receiver;

}

bool ChatService::saveMessage(const QString& sender, const QString& receiver, const QString& message) {
    qDebug() << "Сохранение сообщения в бд:" << sender << "|" << receiver << "|" << message;

    //сохранение в бд

    return true; //заглушка
}
