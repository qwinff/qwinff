#include "ffmpeginterface.h"
#include <QRegExp>
#include <QTextStream>
#include <iostream>

#define TIMEOUT 3000

namespace {
const char mencoder_executable_name[] = "ffmpeg";
namespace patterns {
const char progress[]
    = "size=\\s*([0-9]+)kB\\s+time=\\s*([0-9]+\\.[0-9]+)\\s+bitrate=\\s*([0-9]+\\.[0-9]+)kbits/s";
const char duration[]
    = "Duration:\\s+([0-9][0-9]):([0-9][0-9]):([0-9][0-9](\\.[0-9][0-9]?)?)";
}
}

struct FFmpegInterface::Private
{
    double duration;
    double progress;
    QString stringBuffer;
    QRegExp progress_pattern;
    QRegExp duration_pattern;

    bool encoders_read;
    QList<QString> audio_encoders;
    QList<QString> video_encoders;
    QList<QString> subtitle_encoders;

    Private() : duration(0), progress(0)
      , progress_pattern(patterns::progress)
      , duration_pattern(patterns::duration)
      , encoders_read(false) { }

    bool check_duration(const QString&);
    bool check_progress(const QString&);
    bool read_encoders();
};

/*! Check whether the output line is a progress line.
    If it is, update p->progress and return true.
    Otherwise, keep the value of p->progress and return false.
*/
bool FFmpegInterface::Private::check_progress(const QString& line)
{
    QRegExp& pattern = progress_pattern;
    int index = pattern.indexIn(line);
    if (index != -1) {
        const double t = pattern.cap(2).toFloat();

        // calculate progress
        progress = (t / duration) * 100;

        return true;
    }
    return false;
}

/*! Check whether the output line is the duration line.
    If it is, update p->duration and return true.
    Otherwise, keep the value of p->duration and return false.
*/
bool FFmpegInterface::Private::check_duration(const QString& line)
{
    QRegExp& pattern = duration_pattern;
    int index = pattern.indexIn(line);
    if (index != -1) {
        const int h = pattern.cap(1).toInt();
        const int m = pattern.cap(2).toInt();
        const double s = pattern.cap(3).toDouble();

        duration = h*3600 + m*60 + s;

        return true;
    }
    return false;
}

// Check available encoders by executing "ffmpeg -codecs"
bool FFmpegInterface::Private::read_encoders()
{
    if (encoders_read) return true;

    QProcess ffmpeg_process;
    QStringList command;
    command.push_back(QString("-codecs"));

    ffmpeg_process.setReadChannel(QProcess::StandardOutput);
    ffmpeg_process.start(QString(mencoder_executable_name), command);

    // Wait until ffmpeg has started.
    if (!ffmpeg_process.waitForStarted())
        return false;

    // Wait until ffmpeg has finished.
    if (!ffmpeg_process.waitForFinished(TIMEOUT))
        return false;

    // Find all available encoders
    QRegExp pattern("[ D]E([ VAS])[ S][ D][ T]\\s+([^ ]+)\\s*(.*)$");
    const int AV_INDEX = 1;
    const int CODEC_NAME_INDEX = 2;

    while (ffmpeg_process.canReadLine()) {
        if (pattern.indexIn(ffmpeg_process.readLine()) != -1) {
            QString av = pattern.cap(AV_INDEX);
            QString codec = pattern.cap(CODEC_NAME_INDEX);

            if (av == "A") { // audio encoder
                audio_encoders.push_back(codec);
            } else if (av == "V") { // video encoder
                video_encoders.push_back(codec);
            } else if (av == "S") { // subtitle encoder
                subtitle_encoders.push_back(codec);
            }
        }
    }

    encoders_read = true;
    return true;
}

FFmpegInterface::FFmpegInterface(QObject *parent) :
    ConverterInterface(parent), p(new Private)
{
}

FFmpegInterface::~FFmpegInterface()
{
}

// virtual functions
QString FFmpegInterface::executableName() const
{
    return QString(mencoder_executable_name);
}

void FFmpegInterface::reset()
{
    p->duration = 0;
    p->progress = 0;
    p->stringBuffer.clear();
}

void FFmpegInterface::setReadChannel(QProcess& proc) const
{
    proc.setReadChannel(QProcess::StandardError);
}

void FFmpegInterface::fillParameterList(const ConversionParameters &param, QStringList &list) const
{
    list = param.toFFmpegOptionList();
}

void FFmpegInterface::parseProcessOutput(const QString &data)
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
        if (line.isEmpty()) continue;
        if (p->check_progress(line)) {
            emit progressRefreshed(p->progress);
            continue;
        }
        if (p->check_duration(line)) continue;
    }
}

double FFmpegInterface::progress() const
{
    return p->progress;
}

bool FFmpegInterface::getAudioEncoders(QList<QString> &target) const
{
    if (!p->read_encoders())
        return false;
    target = p->audio_encoders;
    return true;
}

bool FFmpegInterface::getAudioEncoders(QSet<QString> &target) const
{
    QList<QString> encoder_list;
    if (!getAudioEncoders(encoder_list))
        return false;
    target = QSet<QString>::fromList(encoder_list);
    return true;
}

bool FFmpegInterface::getVideoEncoders(QList<QString> &target) const
{
    if (!p->read_encoders())
        return false;
    target = p->video_encoders;
    return true;
}

bool FFmpegInterface::getVideoEncoders(QSet<QString> &target) const
{
    QList<QString> encoder_list;
    if (!getVideoEncoders(encoder_list))
        return false;
    target = QSet<QString>::fromList(encoder_list);
    return true;
}

bool FFmpegInterface::getSubtitleEncoders(QList<QString> &target) const
{
    if (!p->read_encoders())
        return false;
    target = p->subtitle_encoders;
    return true;
}

bool FFmpegInterface::getSubtitleEncoders(QSet<QString> &target) const
{
    QList<QString> encoder_list;
    if (!getSubtitleEncoders(encoder_list))
        return false;
    target = QSet<QString>::fromList(encoder_list);
    return true;
}
