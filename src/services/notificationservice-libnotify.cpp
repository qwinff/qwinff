/*  This file is part of QWinFF, a media converter GUI.

    QWinFF is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 or 3 of the License.

    QWinFF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QWinFF.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "notificationservice-libnotify.h"
#include <libnotify/notify.h>

NotificationService_libnotify::NotificationService_libnotify() : m_success(true)
{
    if (!notify_is_initted())
        m_success = notify_init("QWinFF");
}

NotificationService_libnotify::~NotificationService_libnotify()
{
    /* Do not call notify_uninit() here because other instances
       of this class may be still using the notification system. */
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
