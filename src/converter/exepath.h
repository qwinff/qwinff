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

#ifndef EXEPATH_H
#define EXEPATH_H

#include <QString>
#include <QList>

class ExePath
{
public:
    static void setPath(QString program, QString path);
    static QString getPath(QString program);

    /**
     * Save the paths using QSettings
     */
    static void saveSettings();

    /**
     * Load the paths using QSettings
     */
    static void loadSettings();

    static QList<QString> getPrograms();

private:
    ExePath();
};

#endif // EXEPATH_H
