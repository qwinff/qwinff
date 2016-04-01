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

#include <QHBoxLayout>
#include <QPainter>
#include <QDebug>
#include "progressbar.h"
#include "services/constants.h"

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
    QColor color_fg_gradient_1 = Constants::getColor("ProgressBar/Colors/Foreground/Gradient1");
    QColor color_fg_gradient_2 = Constants::getColor("ProgressBar/Colors/Foreground/Gradient2");
    QColor color_fg_outer_border = Constants::getColor("ProgressBar/Colors/Foreground/OuterBorder");
    QColor color_fg_inner_border = Constants::getColor("ProgressBar/Colors/Foreground/InnerBorder");

    QColor color_text_light = Constants::getColor("ProgressBar/Colors/Text/Light");
    QColor color_text_dark = Constants::getColor("ProgressBar/Colors/Text/Dark");

    QColor color_bg_gradient_1 = Constants::getColor("ProgressBar/Colors/Background/Gradient1");
    QColor color_bg_gradient_2 = Constants::getColor("ProgressBar/Colors/Background/Gradient2");
    QColor color_bg_outer_border = Constants::getColor("ProgressBar/Colors/Background/OuterBorder");
    QColor color_bg_inner_border = Constants::getColor("ProgressBar/Colors/Background/InnerBorder");

    //if (m_percentage >= 0)
    {
        // ===================================================
        // background
        // ===================================================
        QRect background_rect(0, 0, width(), height());
        QLinearGradient background_gradient(0, 0, 0, background_rect.bottom());
        background_gradient.setColorAt(0.0, color_bg_gradient_1);
        background_gradient.setColorAt(1.0, color_bg_gradient_2);
        painter.setBrush(background_gradient);
        painter.setPen(Qt::NoPen);  // !!! Don't draw the border.
        painter.drawRect(background_rect);
        // outer border
        QRect outer_border_rect(0, 0, width()-1, height()-1);
        pen.setColor(color_bg_outer_border);
        painter.setPen(pen);
        painter.drawRect(outer_border_rect);
        // inner border
        QRect inner_border_rect (1, 1, width()-3, height()-3);
        pen.setColor(color_bg_inner_border);
        painter.setPen(pen);
        painter.drawRect(inner_border_rect);

        if (m_percentage > 0) {
            // draw progress bar
            int w = (width()*m_percentage/100)-1;
            int h = height();
            QRect progress_rect(0, 0, w, h);
            QLinearGradient gradient(0, 0, 0, progress_rect.bottom());
            gradient.setColorAt(0, color_fg_gradient_1);
            gradient.setColorAt(1, color_fg_gradient_2);
            painter.setBrush(gradient);
            painter.setPen(Qt::NoPen);  // !!! Don't draw the border.
            painter.drawRect(progress_rect);
            // outer border
            QRect outer_border_rect(0, 0, w-1, h-1);
            pen.setColor(color_fg_outer_border);
            painter.setPen(pen);
            painter.drawRect(outer_border_rect);
            // inner border
            QRect inner_border_rect (1, 1, w-3, w-3);
            pen.setColor(color_fg_inner_border);
            painter.setPen(pen);
            painter.drawRect(inner_border_rect);
        }

        // Restore the pen such that the text can be rendered.
        if (m_percentage > 50) {
            pen.setColor(color_text_dark);
        } else {
            pen.setColor(color_text_light);
        }
        painter.setPen(pen);

        if (!m_show_text) { // show percentage
            painter.drawText(background_rect, QString("%1\%").arg(m_percentage)
                             , QTextOption(Qt::AlignCenter));
        } else { // show custom text
            painter.drawText(background_rect, m_text, QTextOption(Qt::AlignCenter));
        }

        if (m_active) {
            pen.setColor(color_fg_outer_border);
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush); // disable filling
            painter.drawRect(0, 0, width()-1, height()-1);
        }
    }
}
