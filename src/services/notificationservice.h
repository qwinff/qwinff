#ifndef NOTIFICATIONSERVICE_H
#define NOTIFICATIONSERVICE_H

#include <QString>

class NotificationService
{
public:
    /** Send notification
     */
    virtual void send(QString title, QString message) = 0;

    /** Send notification with an image
     *  @param image the path of the image
     */
    virtual void send(QString title, QString message, QString image) = 0;

    /** Determine whether the notification service is available.
     */
    virtual bool serviceAvailable() const = 0;
};

#endif // NOTIFICATIONSERVICE_H
