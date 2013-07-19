#ifndef TESTUPDATEINFOPARSER_H
#define TESTUPDATEINFOPARSER_H

#include <QtTest/QtTest>
#include "../../services/updateinfoparser.h"

class TestUpdateInfoParser : public QObject
{
    Q_OBJECT
private slots:
    void testParseXml();
};

#endif // TESTUPDATEINFOPARSER_H
