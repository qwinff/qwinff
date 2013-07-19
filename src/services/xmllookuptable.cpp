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
#include <QStringList>
#include <QBuffer>
#include "xmllookuptable.h"

XmlLookupTable::XmlLookupTable()
{
}

bool XmlLookupTable::readFile(QIODevice &file)
{
    if (!file.isOpen())
        return false;
    int depth = 0;
    QXmlStreamReader reader(&file);
    QStringList path;

    m_data.clear();
    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartElement) {
            QString name = reader.name().toString();
            QXmlStreamAttributes attrs = reader.attributes();
            path.push_back(name);
            if (reader.readNext() == QXmlStreamReader::Characters) {
                Entry& entry = m_data[path.join("/")];
                entry.data = reader.text().toString().trimmed();
                entry.attributes.clear();
                foreach (QXmlStreamAttribute attr, attrs)
                    entry.attributes[attr.name().toString()]
                            = attr.value().toString();
            }
            ++depth;
        } else if (token == QXmlStreamReader::EndElement) {
            if (depth <= 0)
                return false; // malformed xml
            --depth;
            path.pop_back();
        }
    }

    return !reader.hasError();
}

bool XmlLookupTable::readString(const QString &s)
{
    QByteArray barray = s.toUtf8();
    QBuffer buffer(&barray);
    return readFile(buffer);
}

void XmlLookupTable::setPrefix(const QString &s)
{
    m_prefix = s.trimmed();
    while (m_prefix.endsWith("/")) // remove trailing '/'
        m_prefix.chop(1);
}

QString XmlLookupTable::prefix() const
{
    return m_prefix;
}

QString XmlLookupTable::lookup(const QString &path) const
{
    return m_data[full_path(path)].data;
}

QString XmlLookupTable::operator [](const QString& path) const
{
    return m_data[full_path(path)].data;
}

QString XmlLookupTable::attribute(QString path, QString attr) const
{
    const QMap<QString, QString> &attributes = m_data[full_path(path)].attributes;
    return attributes[attr];
}

void XmlLookupTable::clear()
{
    m_data.clear();
}

QString XmlLookupTable::full_path(const QString &path) const
{
    if (!m_prefix.isEmpty())
        return m_prefix + "/" + path;
    else
        return path;
}
