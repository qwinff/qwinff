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

    static QString getVersion();

private:
    bool m_success;
};

#endif // NOTIFICATIONSERVICE_LIBNOTIFY_H
