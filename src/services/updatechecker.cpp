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

#include "updatechecker.h"
#include "httpdownloader.h"
#include "updateinfoparser.h"
#include "versioncompare.h"
#include "../version.h"
#include "constants.h"

class UpdateChecker::Private
{
public:
    UpdateChecker::CheckResult result;
    QString version;
    QString release_note;
    QString release_date;
    QString download_url;
    HttpDownloader downloader;
    QScopedPointer<UpdateInfoParser> parser;
    Private()
        : result(UpdateChecker::None), parser(new XmlUpdateInfoParser)
    { }
};

UpdateChecker::UpdateChecker(QObject *parent) :
    QObject(parent), p(new Private)
{
    connect(&p->downloader, SIGNAL(downloadFinished(bool,QString,QString)),
            this, SLOT(downloadFinished(bool,QString,QString)));
}

UpdateChecker::~UpdateChecker()
{
}

UpdateChecker::CheckResult UpdateChecker::result() const
{
    return p->result;
}

QString UpdateChecker::versionName() const
{
    return p->version;
}

QString UpdateChecker::releaseDate() const
{
    return p->release_date;
}

QString UpdateChecker::releaseNotes() const
{
    return p->release_note;
}

QString UpdateChecker::downloadUrl() const
{
    return p->download_url;
}

void UpdateChecker::checkUpdate()
{
    QString update_url = Constants::getString("UpdateInfoUrl");
    p->downloader.startDownload(update_url);
}

void UpdateChecker::downloadFinished(bool success, QString /*url*/, QString content)
{
    if (success) {
        if (!p->parser->parse(content)) {
            // parse error
            p->result = DataError;
        } else if (Version(p->parser->version()) > Version(VERSION_STRING)) {
            // new version > current version
            p->result = UpdateFound;
            p->version = p->parser->version();
            p->release_note = p->parser->releaseNotes();
            p->release_date = p->parser->releaseDate();
            p->download_url = p->parser->downloadUrl();
        } else {
            // no new version found
            p->result = UpdateNotFound;
        }
    } else { // failed to connect to server
        p->result = ConnectionError;
    }
    emit receivedResult(static_cast<int>(p->result));
}
