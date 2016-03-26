#ifndef FORMATLABEL_H
#define FORMATLABEL_H

#include <QLabel>
#include <QStringList>

class FormatLabel : public QLabel
{
    Q_OBJECT

public:
    explicit FormatLabel(const QString &fileExt, QWidget *parent = 0);
};



#endif // FORMATLABEL_H
