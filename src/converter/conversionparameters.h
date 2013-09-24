/*  QWinFF - a qt4 gui frontend for ffmpeg
 *  Copyright (C) 2011-2013 Timothy Lin <lzh9102@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

    unsigned int time_begin, time_end;

    /* FFmpeg Specific Options */
    /*! Additional options passed to the ffmpeg transcoder.
        These options will be overriden by other specific options. */
    QString ffmpeg_options;

    /* MEncoder Specific Options */
    //QString mencoder_oac; // output audio codec
    //QString mencoder_ovc; // output video codec
    //QString mencoder_of;  // output format

    /* Speed (FFmpeg + SoX) */
    /*! Turn on/off speed scaling.
        If speed scaling is on, speed_scaling_factor will be used
        to change the speed of the video and audio stream.
     */
    bool speed_scaling;
    /*! Speed scaling factor.
        1.0 is normal speed; less than 1.0, slow down; greater than 1.0, speed up.
        This parameter is only used when speed_scaling is true.
     */
    double speed_scaling_factor;

    /*! Copy all fields except source, destination files from src
     *  @param src the source to copy from
     */
    void copyConfigurationFrom(const ConversionParameters& src);

    /*! Generate a ConversionParameters from ffmpeg command line options.
        This function ignores input and output file options. */
    static ConversionParameters fromFFmpegParameters(const QString& params_str);
    static ConversionParameters fromFFmpegParameters(const char *params_str);

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
        audio_keep_sample_rate = false;
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

        time_begin = time_end = 0;

        ffmpeg_options.clear();

        speed_scaling = false;
        speed_scaling_factor = 1.0;
    }

};

#endif // CONVERSIONPARAMETERS_H
