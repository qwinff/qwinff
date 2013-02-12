#include "ffplayinterface.h"
#include <QProcess>

#ifdef OPERATION_TIMEOUT
#   define TIMEOUT OPERATION_TIMEOUT
#else
#   define TIMEOUT 3000
#endif

#define DEFAULT_WIDTH 320
#define DEFAULT_HEIGHT 180

namespace {
QString ffplay_executable("ffplay");
}

FFplayInterface::FFplayInterface(QObject *parent) :
    QObject(parent), m_proc(new QProcess), m_w(DEFAULT_WIDTH), m_h(DEFAULT_HEIGHT)
{
}

FFplayInterface::~FFplayInterface()
{
    delete m_proc;
}

void FFplayInterface::play(const QString &filename)
{
    ffplay_start(filename, -1, -1);
}

void FFplayInterface::play(const QString &filename, int t_begin, int t_end)
{
    ffplay_start(filename, t_begin, t_end);
}

void FFplayInterface::playFrom(const QString &filename, int t_begin)
{
    ffplay_start(filename, t_begin, -1);
}

void FFplayInterface::playUntil(const QString &filename, int t_end)
{
    ffplay_start(filename, -1, t_end);
}

void FFplayInterface::stop()
{
    if (m_proc->state() != QProcess::NotRunning) {
        m_proc->kill();
        m_proc->waitForFinished(TIMEOUT);
    }
}

void FFplayInterface::setWindowSize(int w, int h)
{
    m_w = w; m_h = h;
}

void FFplayInterface::setWindowTitle(QString str)
{
    m_title = str;
}

void FFplayInterface::setFFplayExecutable(const QString &filename)
{
    ffplay_executable = filename;
}

bool FFplayInterface::FFplayAvailable()
{
    QProcess proc;
    QStringList param;
    /* test whether ffplay could be invoked */
    proc.start(ffplay_executable, param);
    if (!proc.waitForStarted(TIMEOUT))
        return false;
    proc.kill();
    proc.waitForFinished(TIMEOUT);
    return true;
}

/* If t_begin >= 0, start from t_begin; otherwise, start from time zero.
   If t_end >= 0, stop at t_end; othersize, play until end of file. */
void FFplayInterface::ffplay_start(const QString& filename, int t_begin, int t_end)
{
    QStringList param;
    stop();
    param.append("-autoexit");
    param.append("-x"); param.append(QString::number(m_w));
    param.append("-y"); param.append(QString::number(m_h));
    if (!m_title.isEmpty())
        param.append("-window_title"), param.append(m_title);
    if (t_begin >= 0)
        param.append("-ss"), param.append(QString::number(t_begin));
    if (t_end >= 0) {
        param.append("-t"); // duration
        if (t_begin >= 0)
            param.append(QString::number(t_end - t_begin)); // duration = end - begin
        else
            param.append(QString::number(t_end)); // start from time zero
    }
    param.append(filename);
    m_proc->start(ffplay_executable, param);
    m_proc->waitForStarted(TIMEOUT);
}
