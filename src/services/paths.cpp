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
