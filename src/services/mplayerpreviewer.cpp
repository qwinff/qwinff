#include <QProcess>
#include "converter/exepath.h"
#include "mplayerpreviewer.h"

#define TIMEOUT 3000

MPlayerPreviewer::MPlayerPreviewer(QObject *parent) :
    AbstractPreviewer(parent),
    m_proc(new QProcess(this))
{
}

bool MPlayerPreviewer::available() const
{
    QProcess proc;
    QStringList param;
    // test whether mplayer could be invoked
    proc.start(ExePath::getPath("mplayer"), param);
    if (!proc.waitForStarted(TIMEOUT))
        return false;
    proc.kill();
    proc.waitForFinished(TIMEOUT);
    return true;
}

void MPlayerPreviewer::play(const QString& filename)
{
    play(filename, -1, -1);
}

void MPlayerPreviewer::play(const QString &filename, int t_begin, int t_end)
{
    QStringList param;
    stop();
    if (t_begin >= 0) // set begin time: -ss <seconds>
        param.append("-ss"), param.append(QString::number(t_begin));
    if (t_end >= 0) { // set end time: -endpos <seconds>
        param.append("-endpos");
        param.append(QString::number(t_end));
    }
    param.append(filename);
    m_proc->start(ExePath::getPath("mplayer"), param);
    m_proc->waitForStarted(TIMEOUT);
}

void MPlayerPreviewer::playFrom(const QString &filename, int t_begin)
{
    play(filename, t_begin, -1);
}

void MPlayerPreviewer::playUntil(const QString &filename, int t_end)
{
    play(filename, t_end, -1);
}

void MPlayerPreviewer::stop()
{
    m_proc->kill();
    m_proc->waitForFinished(TIMEOUT);
}
