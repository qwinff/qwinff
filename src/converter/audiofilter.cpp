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

#include "audiofilter.h"
#include "conversionparameters.h"
#include "exepath.h"
#include <QProcess>

#ifdef OPERATION_TIMEOUT
#   define TIMEOUT OPERATION_TIMEOUT
#else
#   define TIMEOUT 3000
#endif

/*
  SoX provides much more audio-processing options than
  ffmpeg, yet it doesn't accept as many formats as
  ffmpeg does. In this pipeline, format conversion
  is done by ffmpeg and audio processing is done by SoX.

  The audio-filtering pipeline consists of two stages:

  1. Audio Extraction
     command: ffmpeg -i <infile> -vn -f sox -
     input: infile
     output: stdout
     Extract the audio stream from the input file, convert it
     into sox format, and pipe it to stdout.

  2. Audio Filtering
     command: sox -t sox - -t wav - <options>
     input: stdin
     output: stdout
     Process the audio stream using SoX. The output is piped to
     stdout in Wave format.

*/

AudioFilter::AudioFilter(QObject *parent) :
    QObject(parent),
    m_extractAudioProc(new QProcess(this)),
    m_soxProc(new QProcess(this))
{
}

bool AudioFilter::start(ConversionParameters& params, QProcess *dest)
{
    QStringList ffmpeg_param;
    QStringList sox_param;

    if (m_soxProc->state() != QProcess::NotRunning) {
        m_soxProc->kill();
        m_soxProc->waitForFinished(TIMEOUT);
    }

    if (m_extractAudioProc->state() != QProcess::NotRunning) {
        m_extractAudioProc->kill();
        m_extractAudioProc->waitForFinished(TIMEOUT);
    }

    // ffmpeg process settings
    ffmpeg_param << "-i" << params.source << "-vn" << "-f" << "sox" << "-";
    m_extractAudioProc->setStandardOutputProcess(m_soxProc);

    // sox process settings
    sox_param << "-t" << "sox" << "-" << "-t" << "wav" << "-";
    if (params.speed_scaling) {
        sox_param << "tempo" << QString::number(params.speed_scaling_factor);
    }
    m_soxProc->setStandardOutputProcess(dest);

    // start the two processes
    m_extractAudioProc->start(ExePath::getPath("ffmpeg"), ffmpeg_param);
    m_soxProc->start(ExePath::getPath("sox"), sox_param);

    return m_extractAudioProc->waitForStarted(TIMEOUT)
            && m_soxProc->waitForStarted(TIMEOUT);
}
