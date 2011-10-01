#include "ffmpeginterface.h"
#include <QRegExp>
#include <QTextStream>
#include <QDebug>
#include <QAtomicInt>

// timeout before force-terminating ffmpeg
#ifdef OPERATION_TIMEOUT
#   define TIMEOUT OPERATION_TIMEOUT
#else
#   define TIMEOUT 3000
#endif

namespace {
QString ffmpeg_executable("ffmpeg");
namespace patterns {
    const char progress[]
        = "size=\\s*([0-9]+)kB\\s+time=\\s*([0-9]+\\.[0-9]+)\\s+bitrate=\\s*([0-9]+\\.[0-9]+)kbits/s";
    enum Progress_1_Fields {
        PROG_1_TIME = 2
    };

    const char progress2[] // another possible format where time is represented as hh:mm:ss
        = "size=\\s*([0-9]+)kB\\s+time=\\s*([0-9][0-9]):([0-9][0-9]):([0-9][0-9](\\.[0-9][0-9]?)?)\\s+"
          "bitrate=\\s*([0-9]+\\.[0-9]+)kbits/s";
    enum Progress_2_Fields {
        PROG_2_HR = 2,
        PROG_2_MIN,
        PROG_2_SEC
    };

    const char duration[]
        = "Duration:\\s+([0-9][0-9]):([0-9][0-9]):([0-9][0-9](\\.[0-9][0-9]?)?)";
} // namespace patterns

namespace info {
    QAtomicInt is_encoders_read(false);
    volatile bool ffmpeg_exist = false;
    QString ffmpeg_version;
    QString ffmpeg_codec_info;
    QList<QString> audio_encoders;
    QList<QString> video_encoders;
    QList<QString> subtitle_encoders;

    bool read_ffmpeg_codecs(const char *flag)
    {
        QProcess ffmpeg_process;
        QStringList parameters;
        parameters.push_back(QString(flag));

        ffmpeg_process.setReadChannel(QProcess::StandardOutput);

        qDebug() << ffmpeg_executable << parameters.join(" ");
        ffmpeg_process.start(ffmpeg_executable, parameters);

        // Wait until ffmpeg has started.
        if (!ffmpeg_process.waitForStarted(TIMEOUT)) {
            return false;
        }

        // Wait until ffmpeg has finished.
        if (!ffmpeg_process.waitForFinished(TIMEOUT)) {
            return false;
        }

        if (ffmpeg_process.exitCode() != 0) {
            return false; // error
        }

        // Find all available encoders
        QRegExp pattern("[ D]E([ VAS])[ S][ D][ T]\\s+([^ ]+)\\s*(.*)$");
        const int AV_INDEX = 1;
        const int CODEC_NAME_INDEX = 2;

        ffmpeg_codec_info.clear();
        while (ffmpeg_process.canReadLine()) {
            QString line(ffmpeg_process.readLine());
            ffmpeg_codec_info.append(line);

            if (pattern.indexIn(line) != -1) {
                QString av = pattern.cap(AV_INDEX);
                QString codec = pattern.cap(CODEC_NAME_INDEX);

                if (av == "A") { // audio encoder
                    qDebug() << "Audio Codec: " + codec;
                    audio_encoders.push_back(codec);
                } else if (av == "V") { // video encoder
                    qDebug() << "Video Codec: " + codec;
                    video_encoders.push_back(codec);
                } else if (av == "S") { // subtitle encoder
                    qDebug() << "Subtitle Codec: " + codec;
                    subtitle_encoders.push_back(codec);
                }
            }
        }

        return true;
    }

    bool read_ffmpeg_version()
    {
        QProcess ffmpeg_process;
        QStringList parameters;
        parameters.push_back(QString("-version"));

        qDebug() << ffmpeg_executable << parameters.join(" ");
        ffmpeg_process.start(ffmpeg_executable, parameters);

        ffmpeg_process.waitForStarted(TIMEOUT);
        ffmpeg_process.waitForFinished(TIMEOUT);
        ffmpeg_version = QString(ffmpeg_process.readAll());

        return true;
    }

    /* Read FFmpeg information.
     *  (1) Check available encoder from "ffmpeg -codecs" and "ffmpeg -formats".
     *  (2) Read ffmpeg version information by "ffmpeg -version" command.
     * Once the information is correctly read, it never
     * execute ffmpeg to acquire the information again.
    */
    void read_ffmpeg_info()
    {
        if (!is_encoders_read.testAndSetAcquire(false, true))
            return; // Skip the operation if the information was already read.

        qDebug() << "Read FFmpeg Information";

        /* Older versions of ffmpeg has no "-codecs" flag and report all
         * supported codecs by "-formats". Recent versions report codecs
         * by "-codecs" flag, so we check "-codecs" first. If ffmpeg
         * returns an error, retry with flag "-formats".
         */
        if (!read_ffmpeg_codecs("-codecs") && !read_ffmpeg_codecs("-formats")) {
            is_encoders_read = false; // allow retry when failed
            return;
        }

        if (!read_ffmpeg_version())
            return;

        ffmpeg_exist = true;
    }

} // namespace info
} // anonymous namespace

struct FFmpegInterface::Private
{
    double duration;
    double progress;
    QString stringBuffer;
    QRegExp progress_pattern;
    QRegExp progress_pattern_2;
    QRegExp duration_pattern;

    bool encoders_read;
    QList<QString> audio_encoders;
    QList<QString> video_encoders;
    QList<QString> subtitle_encoders;

    Private() : duration(0), progress(0)
      , progress_pattern(patterns::progress)
      , progress_pattern_2(patterns::progress2)
      , duration_pattern(patterns::duration)
      , encoders_read(false) { }

    bool check_duration(const QString&);
    bool check_progress(const QString&);
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
        const double t = pattern.cap(patterns::PROG_1_TIME).toDouble();

        // calculate progress
        progress = (t / duration) * 100;

        return true;
    } else { // try another pattern
        QRegExp& alternate_pattern = progress_pattern_2;
        if (alternate_pattern.indexIn(line) != -1) {
            const int hour = alternate_pattern.cap(patterns::PROG_2_HR).toInt();
            const int min = alternate_pattern.cap(patterns::PROG_2_MIN).toInt();
            const double sec = alternate_pattern.cap(patterns::PROG_2_SEC).toDouble();
            const double t = hour*3600 + min*60 + sec;

            progress = (t / duration) * 100;

            return true;
        }
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
    return ffmpeg_executable;
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
    qDebug() << data;

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

void FFmpegInterface::setFFmpegExecutable(const QString &filename)
{
    ffmpeg_executable = filename;
}

bool FFmpegInterface::getAudioEncoders(QList<QString> &target)
{
    info::read_ffmpeg_info();
    if (!info::ffmpeg_exist) return false;

    target = info::audio_encoders;
    return true;
}

bool FFmpegInterface::getAudioEncoders(QSet<QString> &target)
{
    QList<QString> encoder_list;
    if (!getAudioEncoders(encoder_list))
        return false;
    target = QSet<QString>::fromList(encoder_list);
    return true;
}

bool FFmpegInterface::getVideoEncoders(QList<QString> &target)
{
    info::read_ffmpeg_info();
    if (!info::ffmpeg_exist) return false;

    target = info::video_encoders;
    return true;
}

bool FFmpegInterface::getVideoEncoders(QSet<QString> &target)
{
    QList<QString> encoder_list;
    if (!getVideoEncoders(encoder_list))
        return false;
    target = QSet<QString>::fromList(encoder_list);
    return true;
}

bool FFmpegInterface::getSubtitleEncoders(QList<QString> &target)
{
    info::read_ffmpeg_info();
    if (!info::ffmpeg_exist) return false;

    target = info::subtitle_encoders;
    return true;
}

QString FFmpegInterface::getFFmpegVersionInfo()
{
    info::read_ffmpeg_info();
    return info::ffmpeg_version;
}

QString FFmpegInterface::getFFmpegCodecInfo()
{
    info::read_ffmpeg_info();
    return info::ffmpeg_codec_info;
}

bool FFmpegInterface::hasFFmpeg()
{
    info::read_ffmpeg_info();
    return info::ffmpeg_exist;
}

void FFmpegInterface::refreshFFmpegInformation()
{
    info::is_encoders_read = false;
    info::read_ffmpeg_info();
}

bool FFmpegInterface::getSubtitleEncoders(QSet<QString> &target)
{
    QList<QString> encoder_list;
    if (!getSubtitleEncoders(encoder_list))
        return false;
    target = QSet<QString>::fromList(encoder_list);
    return true;
}
