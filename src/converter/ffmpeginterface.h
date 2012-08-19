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

#ifndef FFMPEGINTERFACE_H
#define FFMPEGINTERFACE_H
#include <QObject>
#include <QScopedPointer>
#include <QList>
#include <QSet>
#include "conversionparameters.h"
#include "converterinterface.h"

class FFmpegInterface : public ConverterInterface
{
    Q_OBJECT
public:
    explicit FFmpegInterface(QObject *parent = 0);
    virtual ~FFmpegInterface();
    QString executableName() const;
    void reset();
    QProcess::ProcessChannel processReadChannel() const;
    bool needsAudioFiltering(const ConversionParameters& param) const;
    void fillParameterList(const ConversionParameters& param, QStringList& list) const;
    void parseProcessOutput(const QString& data);
    double progress() const;

    static void setFFmpegExecutable(const QString& filename);
    static bool getAudioEncoders(QList<QString>& target);
    static bool getAudioEncoders(QSet<QString>& target);
    static bool getVideoEncoders(QList<QString>& target);
    static bool getVideoEncoders(QSet<QString>& target);
    static bool getSubtitleEncoders(QList<QString>& target);
    static bool getSubtitleEncoders(QSet<QString>& target);
    static QString getFFmpegVersionInfo();
    static QString getFFmpegCodecInfo();
    static bool hasFFmpeg();

    static void refreshFFmpegInformation();

signals:
    void progressRefreshed(double percentage);

public slots:

private:
    Q_DISABLE_COPY(FFmpegInterface)
    struct Private;
    QScopedPointer<Private> p;
};

#endif // FFMPEGINTERFACE_H
