#ifndef PATHS_H
#define PATHS_H

#include <QString>

class Paths
{
public:
    static void setAppPath(const QString& path);
    static QString appPath();

    static QString dataPath();
    static QString translationPath();
};

#endif // PATHS_H
