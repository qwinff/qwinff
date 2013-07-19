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
		"  <DownloadUrl type=\"windows-installer\">downloadurl1</DownloadUrl>"
		"  <DownloadUrl type=\"windows-portable\">downloadurl2</DownloadUrl>"
		"</QWinFFVersionInfo>";
	XmlUpdateInfoParser parser;
	parser.parse(QString(xml));
	QCOMPARE(QString("0.1.8"), parser.version());
	QCOMPARE(QString("20120213"), parser.releaseDate());
	QCOMPARE(QString("release notes"), parser.releaseNotes());
	/* download link not tested because it varies across platforms */
}
