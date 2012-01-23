#ifndef NOTIFICATIONSERVICE_LIBNOTIFY_H
#define NOTIFICATIONSERVICE_LIBNOTIFY_H

#include "notificationservice.h"

/** An implementation of notification functions by using the libnotify library
    directly. This is the most preferred notification mechanism on Linux as
    it is desktop-independent and doesn't require the libnotify command line tools. */
class NotificationService_libnotify : public NotificationService
{
public:
    NotificationService_libnotify();
    ~NotificationService_libnotify();

    virtual void send(QString titie, QString message);

    virtual void send(QString title, QString message, int level);

    virtual bool serviceAvailable() const;

private:
    bool m_success;
};

#endif // NOTIFICATIONSERVICE_LIBNOTIFY_H
