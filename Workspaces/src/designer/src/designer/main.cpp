/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdesigner.h"
#include <QtCore/QLibraryInfo>
#include <QtCore/QDir>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <QThread>
#include <QMainWindow>
#include <stdlib.h>
#include <ConstVar>
#include <Net/NetComponents>
QT_USE_NAMESPACE
QFile s_logFile;
const QString timeFormat = QString::fromUtf8("hh_mm_ss_zzz");
const QString dateFormat = QString::fromUtf8("yyyy_MM_dd");
QString s_logFileNamePattern = g_logFileNamePattern;
void customMessgaeHandler(QtMsgType type,const QMessageLogContext &logcont, const QString& msg)
{
    QTime nowTime = QTime::currentTime();
    QDate nowDate = QDate::currentDate();
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug\t%1\t%2\t%3\t%4\t%5\t%6\t%7\n")
                .arg(nowDate.toString(dateFormat))
                .arg(nowTime.toString(timeFormat))
                .arg(msg)
                .arg(logcont.file)
                .arg(logcont.function)
                .arg(logcont.line)
                .arg(logcont.category);
        break;
    case QtWarningMsg:
        txt = QString("Warning\t%1\t%2\t%3\t%4\t%5\t%6\t%7\n")
                .arg(nowDate.toString(dateFormat))
                .arg(nowTime.toString(timeFormat))
                .arg(msg)
                .arg(logcont.file)
                .arg(logcont.function)
                .arg(logcont.line)
                .arg(logcont.category);
        break;
    case QtCriticalMsg:
        txt = QString("Critical\t%1\t%2\t%3\t%4\t%5\t%6\t%7\n")
                .arg(nowDate.toString(dateFormat))
                .arg(nowTime.toString(timeFormat))
                .arg(msg)
                .arg(logcont.file)
                .arg(logcont.function)
                .arg(logcont.line)
                .arg(logcont.category);
        break;
    case QtFatalMsg:
        txt = QString("Fatal\t%1\t%2\t%3\t%4\t%5\t%6\t%7\n")
                .arg(nowDate.toString(dateFormat))
                .arg(nowTime.toString(timeFormat))
                .arg(msg)
                .arg(logcont.file)
                .arg(logcont.function)
                .arg(logcont.line)
                .arg(logcont.category);
        break;
    default:
        txt = QString("Unknown\t%1\t%2\t%3\t%4\t%5\t%6\t%7\n")
                .arg(nowDate.toString(dateFormat))
                .arg(nowTime.toString(timeFormat))
                .arg(msg)
                .arg(logcont.file)
                .arg(logcont.function)
                .arg(logcont.line)
                .arg(logcont.category);
        break;
    }
    QTextStream ts(&s_logFile);
    ts << txt << endl;
}
//designer的主入口函数
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(designer);
    QDesigner app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    //构建启动画面
    QPixmap indexmap("index.png");
    QRect desktopRect = QApplication::desktop()->screenGeometry();
    QSplashScreen *splash = new QSplashScreen(
                indexmap.scaled(desktopRect.width(),desktopRect.height()));
    //显示启动画面
    splash->showFullScreen();
    app.processEvents();
    //显示启动过程的消息
    Qt::Alignment showTextAlign = Qt::AlignBottom | Qt::AlignCenter;
    splash->showMessage(QObject::tr("Setting up the main window..."),showTextAlign,Qt::red);
    splash->showMessage(QObject::tr("Load Moudles ..."),showTextAlign,Qt::red);
    splash->finish(app.mainWindow());
    delete splash;

    QTime nowTime = QTime::currentTime();
    QDate nowDate = QDate::currentDate();
    TaskInterface * task = NetComponents::getTastCenter();
    s_logFileNamePattern = task->currentTaskPath() + g_logFileNamePattern;
    s_logFile.setFileName(s_logFileNamePattern
                          .arg(nowDate.toString(dateFormat))
                          .arg(nowTime.toString(timeFormat)));
    s_logFile.open(QIODevice::ReadWrite|QIODevice::Append);
    qInstallMessageHandler(customMessgaeHandler);

    int ret = app.exec();
    s_logFile.close();
    //恢复告警信息的事件注册,andrew,20150813
    qInstallMessageHandler(0);
    return ret;
}
