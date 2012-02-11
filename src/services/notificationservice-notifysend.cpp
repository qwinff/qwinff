#include "notificationservice-notifysend.h"
#include <QProcess>
#include <QStringList>

#define NOTIFY_SEND_EXECUTABLE "notify-send"

NotificationService_NotifySend::NotificationService_NotifySend()
{
}

NotificationService_NotifySend::~NotificationService_NotifySend()
{

}

void NotificationService_NotifySend::send(QString title, QString message)
{
    send(title, message, NotifyLevel::INFO);
}

void NotificationService_NotifySend::send(QString title, QString message, int level)
{
    /* notify-send usage:
        notify-send [OPTION...] <SUMMARY> [BODY] - create a notification
       Application Options:
        -u, --urgency=LEVEL               Specifies the urgency level (low, normal, critical).
        -t, --expire-time=TIME            Specifies the timeout in milliseconds at which to expire the notification.
        -i, --icon=ICON[,ICON...]         Specifies an icon filename or stock icon to display.
        -c, --category=TYPE[,TYPE...]     Specifies the notification category.
        -h, --hint=TYPE:NAME:VALUE        Specifies basic extra data to pass. Valid types are int, double, string and byte.
        -v, --version                     Version of the package.
    */

    QProcess proc;
    QStringList options;

    // add icon
    options.append("-i");
    switch (level) {
    case NotifyLevel::INFO:
        options.append("dialog-information");
        break;
    case NotifyLevel::WARNING:
        options.append("dialog-warning");
        break;
    case NotifyLevel::CRITICAL:
        options.append("dialog-error");
        break;
    default: // no icon
        options.pop_back();
    }

    options.append(title);
    options.append(message);

    proc.start(NOTIFY_SEND_EXECUTABLE, options);
    proc.waitForFinished(1000);
}

bool NotificationService_NotifySend::serviceAvailable() const
{
    QProcess proc;
    bool ret;
    proc.start(NOTIFY_SEND_EXECUTABLE);
    ret = proc.waitForStarted(1000);
    proc.kill();
    proc.waitForFinished(1000);
    return ret;
}
