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

#include <QMap>
#include <QVariant>
#include <QXmlStreamReader>
#include <QDebug>
#include <QRegExp>
#include "constants.h"

namespace
{
    bool constants_initialized = false;
    QMap<QString, QVariant> constants;

    bool readLeafElement(QXmlStreamReader& reader)
    {
        Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);
        //QXmlStreamAttributes attrs = reader.attributes();
        QString name = reader.name().toString();

        if (reader.readNext() != QXmlStreamReader::Characters)
            return false; // not a leaf element

        QString data = reader.text().toString().trimmed();
        constants[name] = QVariant(data);

        qDebug() << QString("constant[%1] = %2").arg(name, data);

        return true;
    }

}

bool Constants::readFile(QFile &file)
{
    if (!file.isOpen())
        return false;
    int depth = 0;
    QXmlStreamReader reader(&file);
    constants.clear();
    constants_initialized = false;

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        switch (token) {
        case QXmlStreamReader::StartElement:
            if (reader.name() != "QWinFFConstants") {
                if (depth != 1 || !readLeafElement(reader))
                    return false;
            }
            ++depth;
            break;
        case QXmlStreamReader::EndElement:
            --depth;
            break;
        default:
            break;
        }
    }

    if (!reader.hasError())
        constants_initialized = true;

    return !reader.hasError();
}

bool Constants::getBool(const char *key)
{
    Q_ASSERT(constants_initialized);
    return constants[key].toBool();
}

int Constants::getInteger(const char *key)
{
    Q_ASSERT(constants_initialized);
    return constants[key].toInt();
}

QString Constants::getString(const char *key)
{
    Q_ASSERT(constants_initialized);
    return constants[key].toString().trimmed();
}

QStringList Constants::getSpaceSeparatedList(const char *key)
{
    Q_ASSERT(constants_initialized);
    QString collapsed_string = constants[key].toString()
            .replace(QRegExp("[\n\t ]"), " ");
    return collapsed_string.split(" ", QString::SkipEmptyParts);
}
