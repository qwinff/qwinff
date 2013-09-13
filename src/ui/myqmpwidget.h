#ifndef MYQMPWIDGET_H
#define MYQMPWIDGET_H

#include "qmpwidget/qmpwidget.h"

class MyQMPwidget : public QMPwidget
{
    Q_OBJECT
public:
    explicit MyQMPwidget(QWidget *parent = 0);
    virtual ~MyQMPwidget();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

signals:

public slots:
    void load(const QString &url);
    void pause();

private:
    Q_DISABLE_COPY(MyQMPwidget)
};

#endif // MYQMPWIDGET_H
