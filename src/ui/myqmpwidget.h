/*  QWinFF - a qt4 gui frontend for ffmpeg
 *  Copyright (C) 2011-2013 Timothy Lin <lzh9102@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
