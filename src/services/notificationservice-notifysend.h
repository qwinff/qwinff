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

#ifndef NOTIFICATIONSERVICENOTIFYSEND_H
#define NOTIFICATIONSERVICENOTIFYSEND_H

#include "notificationservice.h"

/** An implementation of notification functions by calling the
    command-line tool "notify-send" to send messages. */
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
