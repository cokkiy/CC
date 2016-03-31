﻿#include "option.h"
#include <QSettings>
#include <QDir>
using namespace std;
//初始化静态成员
Option* Option::Instance = nullptr;
Option::Option(QObject *parent)
    : QObject(parent)
{
    Instance = this;
}

Option::~Option()
{

}

/*!
保存配置
@return void
作者：cokkiy（张立民)
创建时间：2015/12/04 12:42:27
*/
void Option::Save()
{
    QString filePath = QDir::homePath() + QStringLiteral("/.CC-Client/");
    QDir dir(filePath);
    if (!dir.exists())
    {
        dir.mkdir(filePath);
    }
    QString  fileName = filePath + QStringLiteral("CC-Client.ini");
    QSettings  settings(fileName, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue("Interval", this->Interval);
    settings.setValue("IsFirstTimeRun", false);
    QString appPath = QStringLiteral("");
    for (auto& app : StartApps)
    {
        appPath += QStringLiteral("%1 && %2 && %3 && %4|").arg(QString::fromStdString(app.AppPath))
            .arg(QString::fromStdString(app.Arguments))
            .arg(QString::fromStdString(app.ProcessName)).arg(app.AllowMultiInstance);
    }
    settings.setValue("ControlAndMonitor/StartApps", appPath);
    QString procs = QStringLiteral("");
    for (auto& proc : MonitorProcesses)
    {
        procs += QStringLiteral("%1|").arg(proc);
    }
    settings.setValue("ControlAndMonitor/MonitorProc", procs);
}

/*!
加载配置
@return void
作者：cokkiy（张立民)
创建时间：2015/12/04 12:42:50
*/
void Option::Load()
{
    QString  fileName = QDir::homePath() + QStringLiteral("/.CC-Client/CC-Client.ini");
    QSettings  settings(fileName, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    this->Interval = settings.value("Interval", 1).toInt();
    this->IsFirstTimeRun = settings.value("IsFirstTimeRun", true).toBool();
    QString startApp = settings.value("ControlAndMonitor/StartApps", "").toString();
    QStringList appList = startApp.split("|", QString::SkipEmptyParts);
    int index = 0;
    for (auto& path : appList)
    {
        QStringList pathAndArg = path.split(" && ", QString::KeepEmptyParts);
        CC::AppStartParameter param;
        param.ParamId = index;
        param.AppPath = pathAndArg[0].toStdString();
        param.Arguments = pathAndArg[1].toStdString();
        param.ProcessName = pathAndArg[2].toStdString();
        param.AllowMultiInstance = pathAndArg[3].toInt() == 1;
        this->StartApps.push_back(param);
        index++;
    }
    QString proc = settings.value("ControlAndMonitor/MonitorProc", "").toString();
    QStringList procList = proc.split("|", QString::SkipEmptyParts);
    this->MonitorProcesses = procList.toStdList();
}
