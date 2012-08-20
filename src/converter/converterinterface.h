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
    virtual QProcess::ProcessChannel processReadChannel() const = 0;

    /*! Fill parameter list and determine whether the configuration needs
     *  additional audio filtering.
     *  @param param [in] the conversion parameter
     *  @param list [out] the command-line parameter list
     *  @param needs_audio_filter [out] whether AudioFilter should be used. The return value is true
     *          implies that the parameter list makes the conversion process
     *          wait for data input from stdin.
     */
    virtual void fillParameterList(const ConversionParameters& param, QStringList& list
                                   , bool *needs_audio_filter) const = 0;
    virtual void parseProcessOutput(const QString& line) = 0;
    virtual double progress() const = 0;

signals:
    void progressRefreshed(double percentage);

public slots:

};

#endif // CONVERTERINTERFACE_H
