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

#include "exepath.h"
#include <QMap>
#include <QSettings>
#include <QProcess>

#ifdef OPERATION_TIMEOUT
#define TIMEOUT OPERATION_TIMEOUT
#else
#define TIMEOUT 30000
#endif

namespace
{
typedef QMap<QString, QString> Map;
Map program_path;
}

void ExePath::setPath(QString program, QString path)
{
    program_path.insert(program, path);
}

QString ExePath::getPath(QString program)
{
    if (program_path.contains(program))
        return program_path[program];
    else
        Q_ASSERT_X(false, "ExePath::getPath"
                   , QString("Program path of '%1' has not been set.")
                   .arg(program).toStdString().c_str());
    return "";
}

bool ExePath::checkProgramAvailability(QString program)
{
    QProcess proc;
    QStringList param;
    // try to run the program
    proc.start(ExePath::getPath(program), param);
    if (!proc.waitForStarted(TIMEOUT))
        return false; // failed to start the program
    // successfully started the program, kill it immediately
    proc.kill();
    proc.waitForFinished(TIMEOUT);
    return true;
}

void ExePath::saveSettings()
{
    QSettings settings;
    foreach (QString name, program_path.keys()) {
        QString path = program_path[name];
        settings.setValue("exepath/" + name, path);
    }
}

void ExePath::loadSettings()
{
    QSettings settings;
    foreach (QString name, program_path.keys()) {
        QString path = settings.value("exepath/" + name
                                      , program_path[name]).toString();
        program_path[name] = path;
    }
}

QList<QString> ExePath::getPrograms()
{
    return program_path.keys();
}
