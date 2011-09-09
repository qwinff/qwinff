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

signals:

public slots:

private:
    unsigned int m_percentage;
    void paintEvent(QPaintEvent*);

};

#endif // PROGRESSBAR_H
