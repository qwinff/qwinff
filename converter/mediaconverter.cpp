#include "mediaconverter.h"
#include "ffmpeginterface.h"
#include <iostream>

#define TIMEOUT 1000

MediaConverter::MediaConverter(QObject *parent) :
    QObject(parent)
{
    m_pConv = new FFmpegInterface(this);

    connect(&m_proc, SIGNAL(readyRead())
            , this, SLOT(readProcessOutput()));
    connect(&m_proc, SIGNAL(finished(int,QProcess::ExitStatus))
            , this, SLOT(convertProgressFinished(int,QProcess::ExitStatus)));
    connect(m_pConv, SIGNAL(progressRefreshed(double))
            , this, SLOT(convertProgressRefreshed(double)));

}

MediaConverter::~MediaConverter()
{
    this->stop();
}

// public methods

bool MediaConverter::start(ConversionParameters& param)
{
    if (m_proc.state() == QProcess::NotRunning) {
        emit progressRefreshed(0);

        m_pConv->reset();

        QStringList list;
        m_pConv->setReadChannel(m_proc);
        m_pConv->fillParameterList(param, list);

        m_proc.start(m_pConv->executableName(), list);
        return m_proc.waitForStarted(TIMEOUT);
    }
    return false;
}

void MediaConverter::stop()
{
    if (m_proc.state() == QProcess::Running) {
        m_proc.kill();
        m_proc.waitForFinished();
    }
}

double MediaConverter::progress()
{
    return m_pConv->progress();
}

// private slots

void MediaConverter::readProcessOutput()
{
       m_pConv->parseProcessOutput(QString(m_proc.readAll()));
}

void MediaConverter::convertProgressFinished(int exitcode, QProcess::ExitStatus)
{
    emit progressRefreshed(100); // 100% finished
    emit finished(exitcode);
}

void MediaConverter::convertProgressRefreshed(double percentage)
{
    emit progressRefreshed((int)percentage);
}

// private methods
