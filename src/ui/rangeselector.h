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
    int pos_begin();
    int pos_end();
    int pos_to_val(int pos);
    void drawContainer(QPainter& painter, QPen& pen);
    void drawRange(QPainter& painter, QPen& pen);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

    int m_max;
    int m_min;
    int m_val_begin;
    int m_val_end;
    enum {EDGE_BEGIN, EDGE_END} m_dragEdge;
    bool m_mouseDown;
    QPoint m_mousePos;
};

#endif // RANGESELECTOR_H
