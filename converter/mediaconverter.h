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
