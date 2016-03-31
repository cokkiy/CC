﻿/*****************************************************************************
 工作站@class StationManager类实现,实现了开机,关机,重启,启动程序,关闭程序
 等动作
 *****************************************************************************/
#include "stationmanager.h"
#include "IController.h"
#include <Ice/Ice.h>
#include "AppControlParameter.h"
#include <QMessageBox>
#include <QtNetwork>
#include <QHostAddress>
#include <QByteArray>
using namespace std;
using namespace CC;

/*!
创建一个工作站管理类的实例
@param StationList * stations 工作站列表
@param const QModelIndexList & indexs 选择的工作站索引
@param QObject * parent
@return
作者：cokkiy（张立民)
创建时间：2015/11/10 15:43:35
*/
StationManager::StationManager(StationList* stations, const QModelIndexList& indexs /*= QModelIndexList()*/, QObject *parent /*= NULL*/)
    :QObject(parent)
{
    this->pStations = stations;
    this->stationIndexs = indexs;
}

/*!
全部开机
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:53:18
*/
void StationManager::powerOnAll()
{
	for (auto iter = pStations->begin();iter != pStations->end();iter++)
	{
		if (!iter->IsRunning())
		{
			iter->setState(StationInfo::Powering);
			sendPowerOnPacket(&*iter);
		}
	}
}
/*!
开机选择的计算机
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:53:33
*/
void StationManager::powerOn()
{
    for (auto index : stationIndexs)
    {
        StationInfo* s = pStations->atCurrent(index.row());
        s->setState(StationInfo::Powering);
		sendPowerOnPacket(s);
    }
}

//发送开机指令包
void StationManager::sendPowerOnPacket(StationInfo* s)
{
	//开机包格式：6个0xFF加上16个目标网卡MAC地址，共计6+16*6=102个字节
	QUdpSocket udpSocket;
	//向目标机的每个网卡发送开机指令
	QStringList macs = s->MAC().split(",", QString::SkipEmptyParts);
	for (QString& mac : macs)
	{
		QByteArray powerOnData;
		powerOnData.fill(0xFF, 6);
		QByteArray macByte = MAC2Byte(mac);
		for (int i = 0; i < 16; i++)
		{
			powerOnData.append(macByte);
		}

		for (int i = 0; i < 5; i++)
		{
			udpSocket.writeDatagram(powerOnData.data(), powerOnData.size(), QHostAddress::Broadcast, 5555);
		}
	}
}

/*!
重启选择的计算机
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:53:48
*/
void StationManager::reboot()
{
    for (auto index : stationIndexs)
    {
        if (index.column() == 0)
        {
            StationInfo* s = pStations->atCurrent(index.row());
            reboot(s);
        }
    }
}



//重启指定的计算机
void StationManager::reboot(StationInfo* s)
{
    IControllerPrx prx = s->controlProxy;
    if (prx != NULL)
    {
        s->setState(StationInfo::Rebooting);
        try
        {
            //设置监视间隔
            setInterval(1);
            prx->begin_reboot(true, []() {});
        }
        catch (Ice::CommunicatorDestroyedException*)
        {
            s->setState(StationInfo::RebootFailure, QStringLiteral("网络故障,连接通道已关闭"));
        }
        catch (IceUtil::IllegalArgumentException*)
        {
            s->setState(StationInfo::RebootFailure, QStringLiteral("参数错误"));
        }
        catch (Ice::Exception* ex)
        {
            s->setState(StationInfo::RebootFailure, ex->what());
        }
        catch (...)
        {
            s->setState(StationInfo::RebootFailure, QStringLiteral("未知错误"));
        }        
    }
    else
    {
        s->setState(StationInfo::CanNotOperating, QStringLiteral("未连接"));
    }
}

/*!
关闭选择的计算机
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:54:00
*/
void StationManager::shutdown()
{
    for (auto index : stationIndexs)
    {
        if (index.column() == 0)
        {
            StationInfo* s = pStations->atCurrent(index.row());
            shutdown(s);
        }
    }
}

//关闭指定的计算机
void StationManager::shutdown(StationInfo* s)
{
    IControllerPrx prx = s->controlProxy;
    if (prx != NULL)
    {
        s->setState(StationInfo::Shutdowning);
        try
        {
            //设置监视间隔为1秒
            setInterval(1);
            prx->begin_shutdown([]() {});
        }
        catch (Ice::CommunicatorDestroyedException*)
        {
            s->setState(StationInfo::RebootFailure, QStringLiteral("网络故障,连接通道已关闭"));
        }
        catch (IceUtil::IllegalArgumentException*)
        {
            s->setState(StationInfo::RebootFailure, QStringLiteral("参数错误"));
        }
        catch (Ice::Exception* ex)
        {
            s->setState(StationInfo::RebootFailure, ex->what());
        }
        catch (...)
        {
            s->setState(StationInfo::RebootFailure, QStringLiteral("未知错误"));
        }
    }
    else
    {
        s->setState(StationInfo::CanNotOperating, QStringLiteral("未连接"));
    }
}

/*!
重启全部的计算机
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:54:12
*/
void StationManager::rebootAll()
{
    for (auto& s : *pStations)
    {
        reboot(&s);
    }
}
/*!
关闭全部的计算机
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:54:29
*/
void StationManager::shutdownAll()
{
    for (auto& s : *pStations)
    {
        shutdown(&s);
    }
}

/*!
重启程序（指显程序)
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:55:05
*/
void StationManager::restartApp()
{
    for (auto index : stationIndexs)
    {
        if (index.column() == 0)
        {
            StationInfo* s = pStations->atCurrent(index.row());
            restartApp(s);
        }
    }
}

// restart specified station's app
void StationManager::restartApp(StationInfo* s)
{
    IControllerPrx prx = s->controlProxy;
    if (prx != NULL)
    {
        s->setState(StationInfo::AppStarting);
        auto& apps = s->getStartAppNames();
        try
        {
            prx->begin_restartApp(apps,
                [s](const AppStartingResults& results)
            {
                //处理返回结果
                bool ok = true;
                QString message = QStringLiteral("");
                for (auto& result : results)
                {
                    if (result.CtrlResult == FailToStart)
                    {
                        ok = false;
                        message += QStringLiteral("%1启动失败 ").arg(s->getStartAppNameByIndex(result.ParamId));
                    }
                    else if (result.CtrlResult == AlreadyRunning)
                    {
                        message += QStringLiteral("%1已经在运行 ").arg(s->getStartAppNameByIndex(result.ParamId));
                    }

                    //如果自动监视启动进程
                    if (result.CtrlResult == AlreadyRunning || result.CtrlResult == Started)
                    {
                        //添加自动监视进程信息
                    }
                }
                if (ok)
                {
                    s->setState(StationInfo::AppStarted, message);
                }
                else
                {
                    s->setState(StationInfo::AppStartFailure, message);
                }

                //设置新的监视列表
                try
                {
                    s->controlProxy->begin_setWatchingApp(s->getAllShouldMonitoredProcessesName(), []() {});
                }
                catch (...)
                {
                    s->setState(StationInfo::GeneralError, QStringLiteral("设置工作站列表错误"));
                }
            },
                [s](const Ice::Exception& ex)
            {
                AppControlError *  controlError = dynamic_cast<AppControlError*>(const_cast<Ice::Exception*>(&ex));
                if (NULL != controlError)
                {
                    //应用程序启动代理没有启动
                    s->setState(StationInfo::AppStartFailure, QString::fromStdWString(controlError->Reason));
                    return;
                }
                Ice::ConnectionRefusedException *refusedError = dynamic_cast<Ice::ConnectionRefusedException*>(const_cast<Ice::Exception*>(&ex));
                if (NULL != refusedError)
                {
                    //网络故障,无法连接到工作站监控服务程序
                    s->setState(StationInfo::AppStartFailure, QStringLiteral("网络故障,无法连接到工作站监控服务程序"));
                    return;
                }
                //其他未知错误
                s->setState(StationInfo::AppStartFailure, QStringLiteral("未知错误"));
            });
        }
        catch (Ice::CommunicatorDestroyedException*)
        {
            s->setState(StationInfo::AppStartFailure, QStringLiteral("网络故障,连接通道已关闭"));
        }
        catch (IceUtil::IllegalArgumentException*)
        {
            s->setState(StationInfo::AppStartFailure, QStringLiteral("参数错误"));
        }
        catch (Ice::Exception* ex)
        {
            s->setState(StationInfo::AppStartFailure, ex->what());
        }
        catch (...)
        {
            s->setState(StationInfo::AppStartFailure, QStringLiteral("未知错误"));
        }
    }
    else
    {
        s->setState(StationInfo::CanNotOperating, QStringLiteral("未连接"));
    }
}

//mac地址到bytearray
QByteArray StationManager::MAC2Byte(QString mac)
{
	QByteArray result;
	QStringList macByte;
	if (mac.contains(":"))
	{
		macByte = mac.split(":", QString::SkipEmptyParts);
	}
	else
	{
		macByte = mac.split("-", QString::SkipEmptyParts);
	}
	for (QString& b : macByte)
	{
		result.append((byte)b.toUInt(0,16));
	}
	return result;
}

/*!
重启全部工作站程序
@return void
作者：cokkiy（张立民)
创建时间：2015/12/16 18:10:25
*/
void StationManager::restartAllApp()
{
    for (auto& s : *pStations)
    {
        restartApp(&s);
    }
}

/*!
退出程序（指显程序)
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:55:19
*/
void StationManager::exitApp()
{
    for (auto index : stationIndexs)
    {
        if (index.column() == 0)
        {
            StationInfo* s = pStations->atCurrent(index.row());
            exitApp(s);
        }
    }
}

//退出指定工作站的全部远程启动程序
void StationManager::exitApp(StationInfo* s)
{
	list<int> processesId= s->getStartedAppProcessIds();
    if (processesId.empty())
    {
        s->setState(StationInfo::AppNotRunning, QStringLiteral("程序没有运行."));
    }
    else if (s->controlProxy != NULL)
    {
        try
        {
            s->controlProxy->begin_closeApp(processesId,
                [s](const CC::AppControlResults& results)
            {
                for (auto& r : results)
                {
                    if (r == FailToClose || r == Error)
                    {
                        s->setState(StationInfo::AppCloseFailure, QStringLiteral("部分或全部程序无法退出."));
                    }
                }
            },
                [s](const Ice::Exception& ex)
            {
                AppControlError *  controlError = dynamic_cast<AppControlError*>(const_cast<Ice::Exception*>(&ex));
                if (NULL != controlError)
                {
                    //应用程序启动代理没有启动
                    s->setState(StationInfo::AppCloseFailure, QString::fromStdWString(controlError->Reason));
                    return;
                }
                Ice::ConnectionRefusedException *refusedError = dynamic_cast<Ice::ConnectionRefusedException*>(const_cast<Ice::Exception*>(&ex));
                if (NULL != refusedError)
                {
                    //网络故障,无法连接到工作站监控服务程序
                    s->setState(StationInfo::AppCloseFailure, QStringLiteral("网络故障,无法连接到工作站监控服务程序"));
                    return;
                }
                //其他未知错误
                s->setState(StationInfo::AppCloseFailure, QStringLiteral("未知错误"));
            });
        }
        catch (Ice::CommunicatorDestroyedException*)
        {
            s->setState(StationInfo::AppCloseFailure, QStringLiteral("网络故障,连接通道已关闭"));
        }
        catch (IceUtil::IllegalArgumentException*)
        {
            s->setState(StationInfo::AppCloseFailure, QStringLiteral("参数错误"));
        }
        catch (Ice::Exception* ex)
        {
            s->setState(StationInfo::AppCloseFailure, ex->what());
        }
        catch (...)
        {
            s->setState(StationInfo::AppCloseFailure, QStringLiteral("未知错误"));
        }
    }
    else
    {
        s->setState(StationInfo::CanNotOperating, QStringLiteral("未连接"));
    }
}

/*!
退出全部工作站指显程序
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:58:22
*/
void StationManager::exitAllApp()
{
    for (auto& s : *pStations)
    {
        exitApp(&s);
    }
}

/*!
启动程序
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:55:34
*/
void StationManager::startApp()
{
    for (auto index : stationIndexs)
    {
        if (index.column() == 0)
        {
            StationInfo* s = pStations->atCurrent(index.row());
            startApp(s);
        }
    }
}

//start specified stations app
void StationManager::startApp(StationInfo* s)
{
    IControllerPrx prx = s->controlProxy;
    if (prx != NULL)
    {
        s->setState(StationInfo::AppStarting);
        auto& apps = s->getStartAppNames();
        try
        {
            prx->begin_startApp(apps,
                [s](const AppStartingResults& results)
            {
                //处理返回结果
                bool ok = true;
                QString message = QStringLiteral("");
				for (auto& result : results)
				{
					if (result.CtrlResult == FailToStart || result.CtrlResult == Error)
					{
						ok = false;
						message += QStringLiteral("%1启动失败:%2 ")
							.arg(s->getStartAppNameByIndex(result.ParamId))
							.arg(QString::fromStdWString(result.Result));
					}
					else if (result.CtrlResult == AlreadyRunning)
					{
						message += QStringLiteral("%1已经在运行 ").arg(s->getStartAppNameByIndex(result.ParamId));
					}

				}
                if (ok)
                {
                    s->setState(StationInfo::AppStarted, message);
                }
                else
                {
                    s->setState(StationInfo::AppStartFailure, message);
                }

            },
                [s](const Ice::Exception& ex)
            {
                AppControlError *  controlError = dynamic_cast<AppControlError*>(const_cast<Ice::Exception*>(&ex));
                if (NULL != controlError)
                {
                    //应用程序启动代理没有启动
                    s->setState(StationInfo::AppStartFailure, QString::fromStdWString(controlError->Reason));
                    return;
                }
                Ice::ConnectionRefusedException *refusedError = dynamic_cast<Ice::ConnectionRefusedException*>(const_cast<Ice::Exception*>(&ex));
                if (NULL != refusedError)
                {
                    //网络故障,无法连接到工作站监控服务程序
                    s->setState(StationInfo::AppStartFailure, QStringLiteral("网络故障,无法连接到工作站监控服务程序"));
                    return;
                }
                //其他未知错误
                s->setState(StationInfo::AppStartFailure, QStringLiteral("未知错误"));
            });
        }
        catch (Ice::CommunicatorDestroyedException*)
        {
            s->setState(StationInfo::AppStartFailure, QStringLiteral("网络故障,连接通道已关闭"));
        }
        catch (IceUtil::IllegalArgumentException*)
        {
            s->setState(StationInfo::AppStartFailure, QStringLiteral("参数错误"));
        }
        catch (Ice::Exception* ex)
        {
            s->setState(StationInfo::AppStartFailure, ex->what());
        }
        catch (...)
        {
            s->setState(StationInfo::AppStartFailure, QStringLiteral("未知错误"));
        }
    }
    else
    {
        s->setState(StationInfo::CanNotOperating, QStringLiteral("未连接"));
    }
}

/*!
启动所有工作站指显程序
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:57:46
*/
void StationManager::startAllApp()
{
    for (auto& s : *pStations)
    {
        startApp(&s);
    }
}
/*!
强制退出程序
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:56:02
*/
void StationManager::forceExitApp()
{

}


/*!
设置监视间隔
@param int interval 监视间隔(秒)
@return void
作者：cokkiy（张立民)
创建时间：2015/12/01 11:42:42
*/
void StationManager::setInterval(int interval)
{
    for (auto& station : *pStations)
    {
        IControllerPrx prx = station.controlProxy;
        if (prx != NULL)
        {
            prx->begin_setStateGatheringInterval(interval, []() {});                
        }
    }
}


