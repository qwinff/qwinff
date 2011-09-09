#ifndef MEncoderInterface_H
#define MEncoderInterface_H
#include <QObject>
#include <QScopedPointer>
#include <QProcess>
#include <QList>
#include <QSet>
#include "conversionparameters.h"
#include "converterinterface.h"

/* THIS CLASS IS UNFINISHED */

class MEncoderInterface : public ConverterInterface
{
    Q_OBJECT
public:
    explicit MEncoderInterface(QObject *parent = 0);
    virtual ~MEncoderInterface();
    QString executableName() const;
    void reset();
    void setReadChannel(QProcess& proc) const;
    void fillParameterList(const ConversionParameters& param, QStringList& list) const;
    void parseProcessOutput(const QString& data);
    double progress() const;

signals:
    void progressRefreshed(double percentage);

public slots:

private:
    Q_DISABLE_COPY(MEncoderInterface)
    struct Private;
    QScopedPointer<Private> p;
};

#endif // MEncoderInterface_H
