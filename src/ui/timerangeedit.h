#ifndef TIMERANGEEDIT_H
#define TIMERANGEEDIT_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTimeEdit;
class QCheckBox;
QT_END_NAMESPACE

class TimeRangeEdit : public QWidget
{
    Q_OBJECT
public:
    explicit TimeRangeEdit(QWidget *parent = 0);

    int maxTime() const;
    int beginTime() const;
    int endTime() const;
    bool fromBegin() const;
    bool toEnd() const;

public slots:

    /**
     * @brief Set the maximum duration to @a sec
     * @param sec the maximum duration in seconds
     */
    void setMaxTime(int sec);

    /**
     * @brief Set the begin time to @a sec
     */
    void setBeginTime(int sec);

    /**
     * @brief Set the end time to @a sec
     */
    void setEndTime(int sec);

    void setFromBegin(bool);

    void setToEnd(bool);

signals:
    void valueChanged();

private slots:
    void time_changed();

private:
    QTimeEdit *m_timeBegin;
    QTimeEdit *m_timeEnd;
    QCheckBox *m_chkFromBegin;
    QCheckBox *m_chkToEnd;
};

#endif // TIMERANGEEDIT_H
