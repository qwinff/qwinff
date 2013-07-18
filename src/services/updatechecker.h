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

#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QString>
#include <QScopedPointer>
#include <QXmlStreamReader>

/**
 * @brief Get update information from google code.
 */
class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateChecker(QObject *parent = 0);
    ~UpdateChecker();

    enum CheckResult {
        None,
        ConnectionError,
        DataError,
        UpdateNotFound,
        UpdateFound
    };

    CheckResult result() const;
    QString versionName() const;
    QString releaseDate() const;
    QString releaseNotes() const;
    QString downloadUrl() const;
    
signals:
    void receivedResult(int result);
    
public slots:
    void checkUpdate();

private slots:
    void downloadFinished(bool, QString, QString);
    
private:
    Q_DISABLE_COPY(UpdateChecker)
    class Private;
    QScopedPointer<Private> p;
};

#endif // UPDATECHECKER_H
