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

#include <QXmlStreamReader>
#include "updateinfoparser.h"
#include "constants.h"
#include "xmllookuptable.h"

XmlUpdateInfoParser::XmlUpdateInfoParser()
{
}

XmlUpdateInfoParser::~XmlUpdateInfoParser()
{
}

bool XmlUpdateInfoParser::parse(QString s)
{
    XmlLookupTable table;
    if (!table.readString(s))
        return false;
    table.setPrefix("QWinFFVersionInfo");

    m_version = table.lookup("Name");
    m_releaseDate = table.lookup("ReleaseDate");
    m_releaseNotes = table.lookup("ReleaseNotes");
#ifdef Q_OS_WIN32
    if (Constants::getBool("Portable")) {
        m_downloadUrl = table.lookup("WindowsPortable/DownloadLink");
    } else {
        m_downloadUrl = table.lookup("WindowsInstaller/DownloadLink");
    }
#else
    m_downloadUrl = "";
#endif
    m_downloadPage = table.lookup("DownloadPage");

    if (!m_version.isEmpty())
        return true;
    else
        return false;
}

QString XmlUpdateInfoParser::version() const
{
    return m_version;
}

QString XmlUpdateInfoParser::releaseDate() const
{
    return m_releaseDate;
}

QString XmlUpdateInfoParser::releaseNotes() const
{
    return m_releaseNotes;
}

QString XmlUpdateInfoParser::downloadUrl() const
{
    return m_downloadUrl;
}

QString XmlUpdateInfoParser::downloadPage() const
{
    return m_downloadPage;
}
