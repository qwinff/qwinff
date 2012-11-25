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

#ifndef PRESETS_H
#define PRESETS_H

#include <QObject>
#include <QList>
#include <QScopedPointer>

class Preset
{
public:
    unsigned int id;
    QString extension;
    QString label;
    QString category;
    QString parameters;

    /*! Sorting requires less-than operator.
     *  For convenience, the less-than relation is defined as comparing
     *  the ids of the presets.
     */
    bool operator <(const Preset& other) const
    {
        return id < other.id;
    }
};

class Presets : public QObject
{
    Q_OBJECT
public:
    explicit Presets(QObject *parent = 0);
    virtual ~Presets();

    bool readFromFile(const QString& filename, bool removeUnavailableCodecs=true);
    bool readFromFile(const char *filename, bool removeUnavailableCodecs=true);

    bool getExtensions(QList<QString>& target) const;

    bool findPresetById(unsigned int id, Preset& target) const;

    /*! Clear %target and write all presets to it.
     * @param target the list to be written to
     * @return If the function succeeds, it returns true.
     *  Otherwise, it returns false.
     */
    bool getPresets(QList<Preset>& target);

    /*! Clear %target and write presets with the extension to it.
     * @return If the function succeeds, it returns true.
     * @param target the list to be written to
     * @param extension extension
     *  Otherwise, it returns false.
     */
    bool getPresets(const QString& extension, QList<Preset>& target);

    /*! @see getPresets(const QString& extension, QList<Preset>& target) */
    bool getPresets(const char *extension, QList<Preset>& target);

signals:

public slots:

private:
    Q_DISABLE_COPY(Presets)
    struct Private;
    QScopedPointer<Private> p;
};

#endif // PRESETS_H
