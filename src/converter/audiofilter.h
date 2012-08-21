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

#ifndef AUDIOFILTER_H
#define AUDIOFILTER_H

#include <QObject>

class QProcess;
class ConversionParameters;

class AudioFilter : public QObject
{
    Q_OBJECT
public:
    explicit AudioFilter(QObject *parent = 0);

    /**
     * Start the audio-filtering process pipeline.
     * @param param the conversion parameter
     * @param dest the process to receive the output from stdin
     */
    bool start(ConversionParameters& param, QProcess *dest);

    /**
     * Check if execution conditions are met.
     * @return true if AudioFilter works, false otherwise.
     */
    static bool available();

signals:

public slots:

private slots:

private:
    QProcess *m_extractAudioProc;
    QProcess *m_soxProc;
    bool m_useSoxFormat;
};

#endif // AUDIOFILTER_H
