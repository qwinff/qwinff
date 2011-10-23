/*  This file is part of QWinFF, a media converter GUI.

    QWinFF is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 or 3 of the License.

    QWinFF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QWinFF.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONVERSIONPARAMETERS_H
#define CONVERSIONPARAMETERS_H
#include <QString>
#include <QStringList>

class ConversionParameters
{
public:

    /* Generic Options */
    QString source;       ///< name of source file
    QString destination;  ///< name of destination file
    int threads; ///< how many threads to use in conversion

    bool disable_audio;
    int audio_bitrate;
    bool audio_auto_bitrate; ///< keep original bitrate if it is smaller than audio_bitrate
    int audio_sample_rate;
    bool audio_keep_sample_rate; ///< whether to keep original audio sample rate
    int audio_channels;
    int audio_volume; ///< output volume in binary percent (256 is normal, 512 is double)

    bool disable_video;
    bool video_same_quality; ///< whether to keep all video quality (-sameq option in ffmpeg)
    bool video_deinterlace;
    int video_bitrate;
    int video_width;
    int video_height;
    int video_crop_top, video_crop_bottom, video_crop_left, video_crop_right;

    /* FFmpeg Specific Options */
    /*! Additional options passed to the ffmpeg transcoder.
        These options will be overriden by other specific options. */
    QString ffmpeg_options;

    /* MEncoder Specific Options */
    //QString mencoder_oac; // output audio codec
    //QString mencoder_ovc; // output video codec
    //QString mencoder_of;  // output format

    /*! Copy all fields except source and destination files from src
     *  @param src the source to copy from
     */
    void copyConfigurationFrom(const ConversionParameters& src);

    /*! Generate a ConversionParameters from ffmpeg command line options.
        This function ignores input and output file options. */
    static ConversionParameters fromFFmpegParameters(const QString& params_str);
    static ConversionParameters fromFFmpegParameters(const char *params_str);

    QStringList toFFmpegOptionList() const;

    /*! THIS FUNCTION IS UNFINISHED */
    QStringList toMEncoderOptionList() const;

    ConversionParameters()
    {
        clear();
    }

    void clear()
    {
        source.clear();
        destination.clear();
        threads = 0;

        disable_audio = false;
        audio_auto_bitrate = audio_keep_sample_rate = false;
        audio_bitrate = 0;
        audio_sample_rate = 0;
        audio_channels = 0;
        audio_volume = 0;

        disable_video = false;
        video_same_quality = false;
        video_deinterlace = false;
        video_bitrate = 0;
        video_crop_top = video_crop_bottom = video_crop_left = video_crop_right = 0;

        video_width = video_height = 0;

        ffmpeg_options.clear();
    }

};

#endif // CONVERSIONPARAMETERS_H
