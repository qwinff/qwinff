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

#include "paths.h"
#include <QDir>
#include <QDebug>

namespace {
    QString app_path;
}

void Paths::setAppPath(const QString &path)
{
    app_path = path;
    qDebug() << "Set application path: " + app_path;
    qDebug() << "Application Path: " + app_path;
    qDebug() << "Data Path: " + dataPath();
    qDebug() << "translationPath: " + translationPath();
}

QString Paths::appPath()
{
    return app_path;
}

QString Paths::dataPath()
{
#ifdef DATA_PATH
    return QString(DATA_PATH);
#else
    return app_path;
#endif
}

QString Paths::dataFileName(const QString &filename)
{
    return QDir(dataPath()).absoluteFilePath(filename);
}

QString Paths::translationPath()
{
    return QDir(dataPath()).absoluteFilePath("translations");
}
