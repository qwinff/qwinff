#ifndef TESTVERSIONCOMPARE_H
#define TESTVERSIONCOMPARE_H

#include <QtTest/QtTest>
#include "../../services/versioncompare.h"

class TestVersionCompare : public QObject
{
    Q_OBJECT
private slots:
    void testVersionEquality();
    void testVersionInequality();
    void testVersionSingleRange();
};

#endif // TESTVERSIONCOMPARE_H
