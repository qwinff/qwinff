#include <QSettings>
#include <QDateTime>
#include "settingtimer.h"

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
    QSettings().setValue(m_key, QDateTime::currentDateTimeUtc());
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
    QDateTime current_time = QDateTime::currentDateTimeUtc();
    return prev_time.msecsTo(current_time);
}

qint64 SettingTimer::elapsedSeconds() const
{
    const int milliseconds_per_second = 1000;
    return elapsedMilliseconds() / milliseconds_per_second;
}
