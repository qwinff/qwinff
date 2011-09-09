#ifndef CONVERTERINTERFACE_H
#define CONVERTERINTERFACE_H

#include <QObject>
#include <QStringList>
#include <QProcess>
#include "conversionparameters.h"

/**
  Abstract Converter Interface
  Derived class must implement the virtual functions to provide converter details.
  */

class ConverterInterface : public QObject
{
    Q_OBJECT
public:
    explicit ConverterInterface(QObject *parent = 0);
    virtual QString executableName() const = 0;
    virtual void reset() = 0;
    virtual void setReadChannel(QProcess& proc) const = 0;
    virtual void fillParameterList(const ConversionParameters& param, QStringList& list) const = 0;
    virtual void parseProcessOutput(const QString& line) = 0;
    virtual double progress() const = 0;

signals:
    void progressRefreshed(double percentage);

public slots:

};

#endif // CONVERTERINTERFACE_H
