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

#ifndef MEDIACONVERTER_H
#define MEDIACONVERTER_H

#include <QObject>
#include <QProcess>
#include "conversionparameters.h"

class ConversionParameters;
class ConverterInterface;

class MediaConverter : public QObject
{
    Q_OBJECT
public:
    explicit MediaConverter(QObject *parent = 0);
    ~MediaConverter();

    /*!
     * Start the conversion process.
     * @return If the process is successfully started, the function returns true
     *  Otherwise, the function returns false.
     */
    bool start(ConversionParameters param);

    void stop();
    double progress();

signals:
    void finished(int exitcode);
    void progressRefreshed(int percentage);

public slots:

private slots:
    void readProcessOutput();
    void convertProgressFinished(int, QProcess::ExitStatus);
    void convertProgressRefreshed(double);

private:
    Q_DISABLE_COPY(MediaConverter)
    QProcess m_proc;
    ConversionParameters m_convParam;
    ConverterInterface *m_pConv;
    QString m_outputFileName;
    QString m_tmpFileName;
    bool m_stopped;
};

#endif // MEDIACONVERTER_H
