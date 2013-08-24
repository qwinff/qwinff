#include <QSettings>
#include <QDateTime>
#include "settingtimer.h"

namespace
{
qint64 msecsTo(const QDateTime& t1, const QDateTime& t2)
{
    qint64 days = t1.daysTo(t2);
    qint64 msecs = t1.time().msecsTo(t2.time());
    return days * (24*60*60*1000) + msecs;
}
}

SettingTimer::SettingTimer(const QString& key)
    : m_key(key)
{
}

void SettingTimer::start()
{
    restart();
}

void SettingTimer::restart()
{
    QSettings().setValue(m_key, QDateTime::currentDateTime());
}

void SettingTimer::invalidate()
{
    QSettings().remove(m_key);
}

bool SettingTimer::isValid() const
{
    return QSettings().contains(m_key);
}

qint64 SettingTimer::elapsedMilliseconds() const
{
    QDateTime prev_time = QSettings().value(m_key).toDateTime();
    QDateTime current_time = QDateTime::currentDateTime();
    return msecsTo(prev_time, current_time);
}

qint64 SettingTimer::elapsedSeconds() const
{
    const int milliseconds_per_second = 1000;
    return elapsedMilliseconds() / milliseconds_per_second;
}
