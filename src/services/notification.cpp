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

#include "notification.h"
#include "notificationservice-qt.h"
#if defined(Q_WS_X11) && defined(USE_LIBNOTIFY)
 #include "notificationservice-libnotify.h"
#endif
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
            = QSharedPointer<NotificationService>(new NotificationService_qt());
#if defined(Q_WS_X11) && defined(USE_LIBNOTIFY) // libnotify is for X11 desktop
    notify_service[TYPE_LIBNOTIFY]
            = QSharedPointer<NotificationService>(new NotificationService_libnotify());
#endif
    m_type = TYPE_MSGBOX;
}

bool Notification::serviceAvailable(NotificationType type)
{
    if (type < 0 || type >= END_OF_TYPE)
        return false;
    NotificationService *service = notify_service[type].data();
    if (!service)
        return false;
    return service->serviceAvailable();
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
