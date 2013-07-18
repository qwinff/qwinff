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

XmlUpdateInfoParser::XmlUpdateInfoParser()
{
}

XmlUpdateInfoParser::~XmlUpdateInfoParser()
{
}

bool XmlUpdateInfoParser::parse(QString s)
{
    *this = XmlUpdateInfoParser(); // clear all fields
    QXmlStreamReader xml(s);
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;
        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == "QWinFFVersionInfo")
                continue;
            else if (!readLeafElement(xml))
                return false;
        }
    }
    return !xml.hasError();
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

bool XmlUpdateInfoParser::readLeafElement(QXmlStreamReader &xml)
{
    QString name = xml.name().toString();
    QXmlStreamAttributes attributes = xml.attributes();
    xml.readNext();
    if (xml.tokenType() != QXmlStreamReader::Characters)
        return false;
    QString data = xml.text().toString().trimmed();
    if (name == "Name")
        m_version = data;
    else if (name == "ReleaseDate")
        m_releaseDate = data;
    else if (name == "ReleaseNotes")
        m_releaseNotes = data;
    else if (name == "DownloadUrl")
        readDownloadUrl(attributes, data);
    return true;
}

void XmlUpdateInfoParser::readDownloadUrl(QXmlStreamAttributes &attrs, QString url)
{
    (void)attrs; (void)url; // eliminate "unused variable" warning
#ifdef Q_OS_WIN32
    QString type = getAttribute(attrs, "type");
#ifdef PORTABLE_APP
    if (type == "windows-portable")
        m_downloadUrl = url;
#else
    if (type == "windows-installer")
        m_downloadUrl = url;
#endif
#endif
}

QString XmlUpdateInfoParser::getAttribute(QXmlStreamAttributes &attrs, QString name)
{
    foreach (QXmlStreamAttribute attr, attrs) {
        if (attr.name() == name) {
            return attr.value().toString();
        }
    }
    return "";
}
