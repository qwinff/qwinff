#include "progressbar.h"
#include <QHBoxLayout>
#include <QPainter>


ProgressBar::ProgressBar(QWidget *parent) :
    QWidget(parent), m_percentage(0)
{
}

void ProgressBar::setValue(unsigned int value)
{
    if (value != m_percentage) {
        m_percentage = (value <= 100) ? value : 100;
        this->repaint();
    }
}

void ProgressBar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    //if (m_percentage >= 0)
    {
        if (m_percentage > 0) {
            // draw progress bar
            QRect rect_progress(0, 0, (width()*m_percentage/100 - 1), height()-1);
            painter.fillRect(rect_progress, QColor(0,0,255));
        }

        // draw percentage text
        QRect rect_region(0, 0, width()-1, height()-1);
        painter.drawText(rect_region, QString("%1\%").arg(m_percentage)
                         , QTextOption(Qt::AlignCenter));
    }
}
