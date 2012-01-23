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

#ifndef NOTIFICATIONSERVICE_H
#define NOTIFICATIONSERVICE_H

#include <QString>

class NotifyLevel
{
public:
    enum {
        INFO = 0,
        WARNING,
        CRITICAL
    };
};

class NotificationService
{
public:

    /** Send notification
     */
    virtual void send(QString title, QString message) = 0;

    /** Send notification with an image
     *  @param image the path of the image
     */
    virtual void send(QString title, QString message, int level) = 0;

    /** Determine whether the notification service is available.
     */
    virtual bool serviceAvailable() const = 0;
};

#endif // NOTIFICATIONSERVICE_H
