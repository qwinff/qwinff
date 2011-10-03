/* UNFINISHED */

#include "mencoderinterface.h"
#include <QRegExp>
#include <QTextStream>
#include <iostream>

#ifdef OPERATION_TIMEOUT
#   define TIMEOUT OPERATION_TIMEOUT
#else
#   define TIMEOUT 3000
#endif

namespace {
const char mencoder_executable_name[] = "mencoder";
namespace patterns {
const char progress[]
    = "^Pos:\\s*([0-9]+\\.[0-9]+)s\\s*([0-9]+)f\\s*\\(\\s*([0-9]+)%\\)";
const int POSITION_FIELD = 1;
const int FRAME_FIELD = 2;
const int PROGRESS_FIELD = 3;
}
}

struct MEncoderInterface::Private
{
    double progress;
    QString stringBuffer;
    QRegExp progress_pattern;

    bool encoders_read;
    QList<QString> audio_encoders;
    QList<QString> video_encoders;
    QList<QString> subtitle_encoders;

    Private() : progress(0)
      , progress_pattern(patterns::progress)
      , encoders_read(false) { }

    bool check_progress(const QString&);
};

/*! Check whether the output line is a progress line.
    If it is, update p->progress and return true.
    Otherwise, keep the value of p->progress and return false.
*/
bool MEncoderInterface::Private::check_progress(const QString& line)
{
    QRegExp& pattern = progress_pattern;
    int index = pattern.indexIn(line);
    if (index != -1) {
        progress = pattern.cap(patterns::PROGRESS_FIELD).toInt();
        return true;
    }
    return false;
}

MEncoderInterface::MEncoderInterface(QObject *parent) :
    ConverterInterface(parent), p(new Private)
{
}

MEncoderInterface::~MEncoderInterface()
{
}

// virtual functions
QString MEncoderInterface::executableName() const
{
    return QString(mencoder_executable_name);
}

void MEncoderInterface::reset()
{
    p->progress = 0;
    p->stringBuffer.clear();
}

void MEncoderInterface::setReadChannel(QProcess& proc) const
{
    proc.setReadChannel(QProcess::StandardOutput);
}

void MEncoderInterface::fillParameterList(const ConversionParameters &param, QStringList &list) const
{
    list = param.toFFmpegOptionList();
}

void MEncoderInterface::parseProcessOutput(const QString &data)
{
    // split incoming data by [end of line] or [carriage return]
    QStringList lines(data.split(QRegExp("[\r\n]"), QString::KeepEmptyParts));

    if (!p->stringBuffer.isEmpty()) { // prepend buffered data
        lines.front().prepend(p->stringBuffer);
        p->stringBuffer.clear();
    }

    if (!lines.back().isEmpty()) { // buffer incomplete data
        p->stringBuffer = lines.back();
        lines.back().clear();
    }

    QStringList::iterator it = lines.begin();

    for (; it!=lines.end(); ++it) { // parse lines
        QString& line = *it;
        if (!line.isEmpty() && p->check_progress(line)) {
            emit progressRefreshed(p->progress);
        }
    }
}

double MEncoderInterface::progress() const
{
    return p->progress;
}
