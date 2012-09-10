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

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>

class ProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressBar(QWidget *parent = 0);
    ~ProgressBar();

    /*!
      @brief Set the value of the progress
      @param value The percentage of the progress(0~100)
      */
    void setValue(unsigned int value);

    /*!
      @brief Show the string on the progress bar.
      @param str string to display
      @note The string will be displayed until the next value update.
      */
    void showText(const QString& str);

    void setActive(bool active);
    bool isActive() const;

signals:

public slots:

private:
    unsigned int m_percentage;
    bool m_active;
    bool m_show_text;
    QString m_text;
    void paintEvent(QPaintEvent*);

};

#endif // PROGRESSBAR_H
