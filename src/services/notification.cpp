#include "notification.h"
#include "notificationservice-qt.h"
#include "notificationservice-notifysend.h"
#include "notificationservice-libnotify.h"
#include <QSharedPointer>

namespace {
    QList<QSharedPointer<NotificationService> > notify_service;
}

Notification::NotificationType Notification::m_type;

void Notification::init()
{
    for (int i=0; i<END_OF_TYPE; i++)
        notify_service.push_back(QSharedPointer<NotificationService>(0));

    notify_service[TYPE_MSGBOX]
            = QSharedPointer<NotificationService>(new NotificationService_Qt());
    notify_service[TYPE_NOTIFY_SEND]
            = QSharedPointer<NotificationService>(new NotificationService_NotifySend());
    notify_service[TYPE_LIBNOTIFY]
            = QSharedPointer<NotificationService>(new NotificationService_libnotify());

    m_type = TYPE_MSGBOX;
}

bool Notification::serviceAvailable(NotificationType type)
{
    if (type < 0 || type >= END_OF_TYPE)
        return false;
    return notify_service[type]->serviceAvailable();
}

bool Notification::setType(NotificationType type)
{
    if (serviceAvailable(type)) {
        m_type = type;
        return true;
    } else {
        m_type = TYPE_MSGBOX;
        return false;
    }
}

void Notification::send(QString title, QString message)
{
    if (m_type < 0 || m_type >= END_OF_TYPE)
        return;
    notify_service[m_type]->send(title, message);
}

void Notification::send(QString title, QString message, int level)
{
    if (m_type < 0 || m_type >= END_OF_TYPE)
        return;
    notify_service[m_type]->send(title, message, level);
}
