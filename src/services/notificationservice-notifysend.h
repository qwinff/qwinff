#ifndef NOTIFICATIONSERVICENOTIFYSEND_H
#define NOTIFICATIONSERVICENOTIFYSEND_H

#include "notificationservice.h"

/** An implementation of notification functions by command-line tool "notify-send"
    This utility is currently available on unix-like systems only. */
class NotificationService_NotifySend : public NotificationService
{
public:
    NotificationService_NotifySend();

    virtual ~NotificationService_NotifySend();

    virtual void send(QString title, QString message);

    virtual void send(QString title, QString message, int level);

    virtual bool serviceAvailable() const;

};

#endif // NOTIFICATIONSERVICENOTIFYSEND_H
