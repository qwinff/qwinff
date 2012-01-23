#include "notificationservice-libnotify.h"
#include <libnotify/notify.h>

NotificationService_libnotify::NotificationService_libnotify() : m_success(true)
{
    if (!notify_is_initted())
        m_success = notify_init("QWinFF");
}

NotificationService_libnotify::~NotificationService_libnotify()
{
    notify_uninit();
}

void NotificationService_libnotify::send(QString title, QString message)
{
    send(title, message, NotifyLevel::INFO);
}

void NotificationService_libnotify::send(QString title, QString message, int level)
{
    const char *icon;

    // icon
    switch (level) {
    case NotifyLevel::INFO:
        icon = "dialog-information";
        break;
    case NotifyLevel::WARNING:
        icon = "dialog-warning";
        break;
    case NotifyLevel::CRITICAL:
        icon = "dialog-error";
        break;
    default: // no icon
        icon = "";
    }

    NotifyNotification *msg
            = notify_notification_new(title.toLocal8Bit().data()
                                      , message.toLocal8Bit().data(), icon);
    notify_notification_show(msg, 0);
}

bool NotificationService_libnotify::serviceAvailable() const
{
    return m_success;
}
