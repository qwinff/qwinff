/*  This file is part of QWinFF, a media converter GUI.

    QWinFF is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 or 3 of the License.

    QWinFF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QWinFF.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "progressbar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QDebug>

#define COLOR_MARGIN QColor(200,220,255)
#define COLOR_CENTER QColor(150,200,220)
#define COLOR_BORDER QColor(100,150,160)

ProgressBar::ProgressBar(QWidget *parent) :
    QWidget(parent), m_percentage(0), m_active(false), m_show_text(false)
{
    //qDebug() << "ProgressBar Constructor";
}

ProgressBar::~ProgressBar()
{
    //qDebug() << "ProgressBar Destructor";
}

void ProgressBar::setValue(unsigned int value)
{
    if (value != m_percentage) {
        m_percentage = (value <= 100) ? value : 100;
        this->repaint();
    }
    m_show_text = false;
}

void ProgressBar::showText(const QString &str)
{
    m_show_text = true;
    m_text = str;
    this->repaint();
}

void ProgressBar::setActive(bool active)
{
    if (m_active != active) {
        m_active = active;
        this->repaint();
    }
}

bool ProgressBar::isActive() const
{
    return m_active;
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

        QRect rect_region(0, 0, width()-1, height()-1);
        if (!m_show_text) { // show percentage
            painter.drawText(rect_region, QString("%1\%").arg(m_percentage)
                             , QTextOption(Qt::AlignCenter));
        } else { // show custom text
            painter.drawText(rect_region, m_text, QTextOption(Qt::AlignCenter));
        }

        if (m_active) {
            pen.setColor(COLOR_BORDER);
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush); // disable filling
            painter.drawRect(0, 0, width()-1, height()-1);
        }
    }
}
