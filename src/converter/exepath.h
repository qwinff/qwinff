#ifndef EXEPATH_H
#define EXEPATH_H

#include <QString>

class ExePath
{
public:
    static void setPath(QString program, QString path);
    static QString getPath(QString program);

private:
    ExePath();
};

#endif // EXEPATH_H
