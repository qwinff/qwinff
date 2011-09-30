#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>

class ProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressBar(QWidget *parent = 0);

    /*!
      @brief Set the value of the progress
      @param value The percentage of the progress(0~100)
      */
    void setValue(unsigned int value);

    void setActive(bool active);
    bool isActive() const;

signals:

public slots:

private:
    unsigned int m_percentage;
    bool m_active;
    void paintEvent(QPaintEvent*);

};

#endif // PROGRESSBAR_H
