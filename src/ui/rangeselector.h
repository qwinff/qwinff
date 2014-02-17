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

#ifndef RANGESELECTOR_H
#define RANGESELECTOR_H

#include <QWidget>

class RangeSelector : public QWidget
{
    Q_OBJECT
public:
    explicit RangeSelector(QWidget *parent = 0);
    ~RangeSelector();

    int beginValue() const;
    int endValue() const;
    int minValue() const;
    int maxValue() const;

signals:
    void valueChanged();
    void beginValueChanged(int newvalue);
    void endValueChanged(int newvalue);

public slots:
    void setBeginValue(int beginValue);
    void setEndValue(int beginValue);
    void setMaxValue(int maxValue);

private slots:
    void mouseDown(QPoint pos);
    void mouseDrag(QPoint newpos);
    void mouseClick(QPoint pos);

private:
    Q_DISABLE_COPY(RangeSelector)
    enum Edge {EDGE_BEGIN, EDGE_END};
    int pos_begin();
    int pos_end();
    int pos_to_val(int pos);
    Edge edgeToMove(const QPoint &pos);
    void drawContainer(QPainter& painter, QPen& pen);
    void drawRange(QPainter& painter, QPen& pen);
    void drawCursorPosition(QPainter& painter, QPen& pen);
    void drawHorizontalArrow(int beginX, int endX, QPainter& painter, QPen& pen);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

    int m_max;
    int m_min;
    int m_val_begin;
    int m_val_end;
    Edge m_dragEdge;
    bool m_mouseDown;
    bool m_mouseIsInsideWidget;
    QPoint m_mouseDownPos;
    QPoint m_mousePos;
};

#endif // RANGESELECTOR_H
