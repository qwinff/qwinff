#include "progressbar.h"
#include <QHBoxLayout>
#include <QPainter>

#define COLOR_MARGIN QColor(200,220,255)
#define COLOR_CENTER QColor(150,200,220)

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
    QPen pen;

    //if (m_percentage >= 0)
    {
        if (m_percentage > 0) {
            // draw progress bar
            QRect rect_progress(0, 0, (width()*m_percentage/100)-1, height()-1);
            QLinearGradient gradient(0, 0, 0, rect_progress.bottom());
            gradient.setColorAt(0, COLOR_MARGIN);
            gradient.setColorAt(0.5, COLOR_CENTER);
            gradient.setColorAt(1, COLOR_MARGIN);
            painter.setBrush(gradient);
            painter.setPen(Qt::NoPen);  // Don't draw the border.
            painter.drawRect(rect_progress);
        }

        // Restore the pen such that the text can be rendered.
        painter.setPen(pen);

        // draw percentage text
        QRect rect_region(0, 0, width()-1, height()-1);
        painter.drawText(rect_region, QString("%1\%").arg(m_percentage)
                         , QTextOption(Qt::AlignCenter));
    }
}
