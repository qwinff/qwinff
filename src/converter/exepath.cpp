#include "exepath.h"
#include <QMap>

namespace
{
QMap<QString, QString> program_path;
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
