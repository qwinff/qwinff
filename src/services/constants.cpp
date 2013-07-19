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

#define REGEXP_HEXDIGIT "[0-9a-fA-F]"

namespace
{
    bool constants_initialized = false;
    QMap<QString, QVariant> constants;
    QString color_pattern(QString("#(%1%1)(%1%1)(%1%1)(%1%1)?").arg(REGEXP_HEXDIGIT));

    void processData(const QXmlStreamAttributes& /*attrs*/,
                            const QString& name,
                            const QString& data)
    {
        constants[name] = QVariant(data);
        qDebug() << QString("constant[%1] = %2").arg(name, data);
    }

    int hex2int(const QString& hex_str)
    {
        QString qualified_str = QString("0x%1").arg(hex_str);
        bool ok;
        int value = qualified_str.toInt(&ok, 16);
        if (ok)
            return value;
        else
            return 0;
    }

    QColor str2color(const QString& color_str)
    {
        QRegExp color(color_pattern);
        if (color.indexIn(color_str) >= 0) {
            int r_value = hex2int(color.cap(1));
            int g_value = hex2int(color.cap(2));
            int b_value = hex2int(color.cap(3));
            int a_value = 0xff;
            if (!color.cap(4).isEmpty()) // with alpha value
                a_value = hex2int(color.cap(4));
            return QColor(r_value, g_value, b_value, a_value);
        } else {
            return QColor(0, 0, 0); // default black
        }
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
    QStringList path;

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartElement) {
            QString name = reader.name().toString();
            if (depth > 0) {
                path.push_back(name);
                QXmlStreamAttributes attrs = reader.attributes();
                if (reader.readNext() == QXmlStreamReader::Characters) {
                    QString data = reader.text().toString().trimmed();
                    processData(attrs, path.join("/"), data);
                }
            }
            ++depth;
        } else if (token == QXmlStreamReader::EndElement) {
            --depth;
            if (depth > 0) {
                path.pop_back();
            }
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

QColor Constants::getColor(const char *key)
{
    Q_ASSERT(constants_initialized);
    return str2color(constants[key].toString());
}
