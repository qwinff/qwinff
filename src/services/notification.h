#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QString>

class Notification
{
public:
    enum NotificationType {
        TYPE_QT = 0, ///< use qt built-in tray icon notification
        TYPE_NOTIFY_SEND, ///< use the "notify-send" command line utility
        END_OF_TYPE
    };

    /** Initialize the notification subsystem.
     *  @warning This function is not thread-safe. Only call it in the main thread.
     */
    static void init();

    /** Determine whether the notification type is available in the system.
     */
    static bool serviceAvailable(NotificationType type);

    /** Decide how the notification will be sent.
     *  @param type the type of the notification
     *  @return If the function succeeds, the function returns true.
     *          Otherwise, the function returns false and fallback to TYPE_QT.
     */
    static bool setType(NotificationType type);

    static void send(QString title, QString message);

    static void send(QString title, QString message, QString image);
private:
    static NotificationType m_type;
};

#endif // NOTIFICATION_H
