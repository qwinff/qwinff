#include "paths.h"
#include <QDir>
#include <QDebug>

namespace {
    QString app_path;
}

void Paths::setAppPath(const QString &path)
{
    app_path = path;
    qDebug() << "Set application path: " << app_path;
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

QString Paths::translationPath()
{
#ifdef TRANSLATION_PATH
    return QString(TRANSLATION_PATH);
#else
    return QDir(app_path).absoluteFilePath("translation");
#endif
}
