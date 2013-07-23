#ifndef SETTINGTIMER_H
#define SETTINGTIMER_H

#include <QString>

/**
 * @brief timer using QSettings to maintain the time across sessions
 */
class SettingTimer
{
public:
    SettingTimer(const QString& key);
    void start();
    void restart();
    void invalidate();
    bool isValid() const;
    qint64 elapsedMilliseconds() const;
    qint64 elapsedSeconds() const;
private:
    QString m_key;
};

#endif // SETTINGTIMER_H
