#include "myqmpwidget.h"

MyQMPwidget::MyQMPwidget(QWidget *parent) :
    QMPwidget(parent)
{

}

MyQMPwidget::~MyQMPwidget()
{
}

void MyQMPwidget::mouseDoubleClickEvent(QMouseEvent */*event*/)
{
    // disable mouse doubleclick event by not passing it to QMPwidget
}

void MyQMPwidget::keyPressEvent(QKeyEvent */*event*/)
{
    // disable key events by not passing them to QMPwidget
}

void MyQMPwidget::load(const QString &url)
{
    QMPwidget::load(url);
}

void MyQMPwidget::pause()
{
    QMPwidget::writeCommand("pause");
}
