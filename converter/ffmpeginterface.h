#ifndef FFMPEGINTERFACE_H
#define FFMPEGINTERFACE_H
#include <QObject>
#include <QScopedPointer>
#include <QProcess>
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
    void setReadChannel(QProcess& proc) const;
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
