#include "testupdateinfoparser.h"

QTEST_MAIN(TestUpdateInfoParser)

void TestUpdateInfoParser::testParseXml()
{
	const char *xml =
		"<?xml version=\"1.0\"?>"
		"<QWinFFVersionInfo>"
		"<Name>0.1.8</Name>"
		"<ReleaseDate>20120213</ReleaseDate>"
		"<ReleaseNotes>release notes</ReleaseNotes>"
		"</QWinFFVersionInfo>";
	XmlUpdateInfoParser parser;
    bool success = parser.parse(QString(xml));
    QVERIFY(success);
	QCOMPARE(QString("0.1.8"), parser.version());
	QCOMPARE(QString("20120213"), parser.releaseDate());
	QCOMPARE(QString("release notes"), parser.releaseNotes());
	/* download link not tested because it varies across platforms */
}
