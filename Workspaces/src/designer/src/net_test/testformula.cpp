/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include "../net/LFormula.h"
#include "../net/selfshare/src/config/config.h"
class TestLFormula: public QObject
{
    Q_OBJECT

private slots:
    void ComputeValue_data();
    void ComputeValue();
};

void TestLFormula::ComputeValue_data()
{
    QTest::addColumn<QString>("comp");
    QTest::addColumn<QString>("result");

    QTest::newRow("add") << "1+2" << "3";
    QTest::newRow("sub") << "4-1" << "3";
    QTest::newRow("mul") << "2*6" << "12";
    QTest::newRow("div") << "2/4" << "0.5";
    QTest::newRow("and") << "1&0xff" << "1";
    QTest::newRow("or") << "1|0xff" << "255";
    QTest::newRow("mix") << "(4-1)/2" << "1.5";
}

void TestLFormula::ComputeValue()
{
    QFETCH(QString, comp);
    QFETCH(QString, result);
    Config c;
    c.LoadConfig("");
    LFormula pf(&c);
    double ret = 0;
    if(pf.compute(comp.toStdString(),ret))
    {
        QCOMPARE(QString("%1").arg(ret), result);
    }
    else
    {

    }
}

QTEST_MAIN(TestLFormula)
#include "testlformula.moc"
