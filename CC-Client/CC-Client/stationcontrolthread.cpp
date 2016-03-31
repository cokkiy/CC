#include "stationcontrolthread.h"
#include <list>
#include <Ice/LocalException.h>
using namespace CC;
using namespace std;


void StationControlThread::startApp(StationInfo* s)
{
    //每次启动程序,都清空远程启动程序监视列表
    s->clearTempMonitorProcess();

    IControllerPrx prx = s->controlProxy;
    if (prx != NULL)
    {
        s->setState(StationInfo::AppStarting);
        auto& apps = s->getStartAppNames();
        list<AppStartParameter> params;
        for (auto& app : apps)
        {
            params.push_back(AppStartParameter{ app.first.toStdString(),app.second.toStdString() });
        }
        try
        {
            prx->begin_startApp(params,
                [s](const AppStartingResults& results)
            {
                //处理返回结果
                bool ok = true;
                QString message = QStringLiteral("");
                for (auto& result : results)
                {
                    if (result.Result == FailToStart)
                    {
                        ok = false;
                        message += QStringLiteral("%1启动失败 ").arg(QString::fromStdString(result.AppName));
                    }
                    else if (result.Result == AlreadyRunning)
                    {
                        message += QStringLiteral("%1已经在运行 ").arg(QString::fromStdString(result.AppName));
                    }

                    //如果自动监视启动进程
                    if (s->autoMonitorRemoteStartApp && (result.Result == AlreadyRunning || result.Result == Started))
                    {
                        //添加自动监视进程信息
                        s->addTempMonitorProcess(result.AppName, result.Id);
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
                    s->controlProxy->setWatchingApp(s->getAllMonitorProc());
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
            s->setState(StationInfo::PowerOffOrNetworkFailure, QStringLiteral("连接通道已关闭"));
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
}
