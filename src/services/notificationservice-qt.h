#ifndef NOTIFICATIONSERVICEQT_H
#define NOTIFICATIONSERVICEQT_H

#include "notificationservice.h"

/** An implementation of notification functions by an always-on-top, non-modal QMessageBox */
class NotificationService_Qt : public NotificationService
{
public:
    NotificationService_Qt();
    virtual ~NotificationService_Qt();

    virtual void send(QString titie, QString message);

    virtual void send(QString title, QString message, int level);

    virtual bool serviceAvailable() const;

};

#endif // NOTIFICATIONSERVICEQT_H
