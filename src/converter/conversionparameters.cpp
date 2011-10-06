#include "conversionparameters.h"
#include "mediaprobe.h"
#include <QStringList>
#include <QDebug>
#include <cassert>

#define TIMEOUT 3000

namespace {
int fillArgument(QStringList& args, int index, ConversionParameters& result) {
    int used_arg_count = 0;
    try {
        QString& arg = args[index];

        /* BEGIN MACRO DEFINITIONS */

#define CHECK_OPTION_BEGIN if (false) do { } while (0)

#define CHECK_OPTION(argument) else if (arg == argument)

// absorb options like "-option" with no parameter
// set result.property to value
#define CHECK_OPTION_1(argument, property, value) \
    else if (arg == argument) do { result.property = value; used_arg_count = 1; } while (0)

// absorb options like "-option str" where "str" is a string.
// set result.property to the next token
#define CHECK_OPTION_2(argument, property) \
    else if (arg == argument) do { \
        result.property = args[index+1]; \
        used_arg_count = 2; } while (0)

// absorb options like "-option value" where "value" has numeric meaning
// convertmethod is the corresponding conversion method of QString
// set result.property to the converted value of the next token
#define CHECK_OPTION_2_CONVERT(argument, property, convertmethod) \
    else if (arg == argument) do { \
        QString nextarg = args[index+1]; \
        nextarg.replace(QRegExp("[a-z]"), ""); /* remove units */ \
        result.property = nextarg.convertmethod(); \
        used_arg_count = 2; } while (0)

#define CHECK_OPTION_END do { } while (0)

        /* END MACRO DEFINITIONS */

        if (arg[0] == '-') {

            CHECK_OPTION_BEGIN;

            // Audio Options
            CHECK_OPTION_1("-an", disable_audio, true);
            CHECK_OPTION_2_CONVERT("-ab", audio_bitrate, toInt);
            CHECK_OPTION_2_CONVERT("-ar", audio_sample_rate, toInt);
            CHECK_OPTION_2_CONVERT("-ac", audio_channels, toInt);
            CHECK_OPTION_2_CONVERT("-vol", audio_volume, toInt);

            // Video Options
            CHECK_OPTION_1("-vn", disable_video, true);
            CHECK_OPTION_1("-sameq", video_same_quality, true);
            CHECK_OPTION_1("-deinterlace", video_deinterlace, true);
            CHECK_OPTION_2_CONVERT("-b", video_bitrate, toInt);
            CHECK_OPTION_2_CONVERT("-croptop", video_crop_top, toInt);
            CHECK_OPTION_2_CONVERT("-cropbottom", video_crop_bottom, toInt);
            CHECK_OPTION_2_CONVERT("-cropleft", video_crop_left, toInt);
            CHECK_OPTION_2_CONVERT("-cropright", video_crop_right, toInt);

            // width and height are in the same parameter (for example: "-s 800x600")
            CHECK_OPTION("-s") {
                QRegExp pattern("([0-9]+)x([0-9]+)");
                if (pattern.indexIn(args[index+1]) != -1) {
                    result.video_width = pattern.cap(1).toInt();
                    result.video_height = pattern.cap(2).toInt();
                    used_arg_count = 2;
                }
            }

            CHECK_OPTION_END;

        }

    } catch (...) {
        return 0;
    }
    return used_arg_count;

#undef CHECK_OPTION_BEGIN
#undef CHECK_OPTION_1
#undef CHECK_OPTION_2
#undef CHECK_OPTION_2_CONVERT
#undef CHECK_OPTION_END
}
}

void ConversionParameters::copyConfigurationFrom(const ConversionParameters &src)
{
    QString orig_src = source;
    QString orig_dest = destination;
    *this = src;
    source = orig_src;
    destination = orig_dest;
}

ConversionParameters
ConversionParameters::fromFFmpegParameters(const QString &params_str)
{
    ConversionParameters result;
    QStringList args = params_str.split(" ", QString::SkipEmptyParts);

    for (int i=0; i<args.size();) {
        int used_arg_count = fillArgument(args, i, result);

        if (used_arg_count) {
            for (int k=0; k<used_arg_count; k++)
                args.removeAt(i); // remove recognized arguments
        } else {
            i++;
        }
    }

    result.ffmpeg_options = args.join(" "); // unrecognized arguments

    return result;
}

ConversionParameters
ConversionParameters::fromFFmpegParameters(const char *params_str)
{
    return fromFFmpegParameters(QString(params_str));
}

// convert
QStringList ConversionParameters::toFFmpegOptionList() const
{
    MediaProbe probe;

    if (audio_keep_bitrate || audio_keep_sample_rate) {
        // Probe the bitrate of the input file and apply the value to output.
        probe.start(source);
        probe.wait(TIMEOUT);
    }

    QStringList list;

    // overwrite if file exists
    list.append("-y");

    // source file
    list.append("-i");
    list.append(source);

    /* ==== Additional Options ==== */
    if (!ffmpeg_options.isEmpty()) {
        QList<QString> additional_options =
                ffmpeg_options.split(" ", QString::SkipEmptyParts);
        list.append(additional_options);
    }

    /* ==== Audio/Video Options ==== */

    // Audio Options
    if (disable_audio) {
        list.append("-an"); // no audio
    } else { // audio enabled

        // audio bitrate in kb/s
        if (audio_bitrate > 0) {
            list.append("-ab");

            int bitrate = audio_bitrate;
            if (audio_keep_bitrate && !probe.error() && probe.audioBitRate() != 0) {
                bitrate = probe.audioBitRate(); // apply probed bitrate
                qDebug() << "Apply probed bitrate: " + QString::number(bitrate);
            }

            list.append(QString("%1k").arg(bitrate));
        }

        // audio sample rate in hz
        if (audio_sample_rate > 0) {
            list.append("-ar");

            int sample_rate = audio_sample_rate;
            if (audio_keep_sample_rate && !probe.error() && probe.audioSampleRate() != 0) {
                sample_rate = probe.audioSampleRate();
                qDebug() << "Apply probed sample rate: " + QString::number(sample_rate);
            }

            list.append(QString("%1").arg(sample_rate));
        }

        // audio channels
        if (audio_channels > 0) {
            list.append("-ac");
            list.append(QString("%1").arg(audio_channels));
        }

        // volume
        if (audio_volume > 0) {
            list.append("-vol");
            list.append(QString("%1").arg(audio_volume));
        }

    }

    // Video Options
    if (disable_video) {
        list.append("-vn"); // no video
    } else { // video enabled

        // same video quality as source
        if (video_same_quality) {
            list.append("-sameq");
        }

        // deinterlace
        if (video_deinterlace) {
            list.append("-deinterlace");
        }

        // video bitrate
        if (video_bitrate > 0) {
            list.append("-b");
            list.append(QString("%1k").arg(video_bitrate));
        }

        // video dimensions
        if (video_width > 0 && video_height > 0) {
            list.append("-s");
            list.append(QString("%1x%2").arg(video_width).arg(video_height));
        }

        if (video_crop_top > 0) {
            list.append("-croptop");
            list.append(QString("%1").arg(video_crop_top));
        }

        if (video_crop_bottom > 0) {
            list.append("-cropbottom");
            list.append(QString("%1").arg(video_crop_bottom));
        }

        if (video_crop_left > 0) {
            list.append("-cropleft");
            list.append(QString("%1").arg(video_crop_left));
        }

        if (video_crop_right > 0) {
            list.append("-cropright");
            list.append(QString("%1").arg(video_crop_right));
        }

    }

    // destination file
    list.append(destination);

    return list;
}

/* THIS FUNCTION IS UNFINISHED
   Remove this notice once you finish it */
QStringList ConversionParameters::toMEncoderOptionList() const
{
    assert(!"This function has not been finished.");
    QStringList list;

    // source file
    list.append(source);
    // destination file
    list.append("-o");
    list.append(destination);

    /* Audio Options */


    /* Video Options */


}
