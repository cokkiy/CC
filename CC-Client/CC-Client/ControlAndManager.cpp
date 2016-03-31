#include "ControlAndManager.h"
#include "StationStateReceiver.h"
#include <list>
#include <thread>
using namespace CC;
using namespace std;


ControlAndManager::ControlAndManager(StationList* pStations, QObject *parent)
    : QThread(parent)
{
    this->pStations = pStations;
}

ControlAndManager::~ControlAndManager()
{

}

/*!
线程核心函数,负责接收控制命令,并向工作站发送
@return void
作者：cokkiy（张立民)
创建时间：2015/12/15 15:56:35
*/
void ControlAndManager::run()
{
    while (true)
    {
        if (pStations->total() > 0)
        {
            list<AppStartParameter> params;
            params.push_back(AppStartParameter{ "/home/cokkiy/Projects/build-qttools-gcc-5.2/bin/designer","" });
            startApp(params, &*pStations->begin());

        }
        std::this_thread::sleep_for(chrono::seconds(5));
    }
}

/*!
启动工作站状态接收并启动命令接收线程
@param QThread::Priority priority
@return void
作者：cokkiy（张立民)
创建时间：2015/12/15 15:58:22
*/
void ControlAndManager::start(QThread::Priority priority /*= QThread::InheritPriority*/)
{
    int argc = 0;
    Ice::InitializationData initData;
    initData.properties = Ice::createProperties();
    //加载配置文件
    initData.properties->load("Config.Server");
    communicator = Ice::initialize(argc, 0, initData);
    adapter = communicator->createObjectAdapter("StateReceiver");
    auto StationStateReceiverPtr = new StationStateReceiver(pStations);
    adapter->add(StationStateReceiverPtr, communicator->stringToIdentity("stateReceiver"));
    adapter->activate();
    QThread::start(priority);
}

void ControlAndManager::startApp(CC::AppStartParameters startParams, StationInfo *pStation)
{
    //每次启动程序,都清空远程启动程序监视列表
    pStation->clearTempMonitorProcess();

    IControllerPrx prx = pStation->controlProxy;
    if (prx != NULL)
    {
        pStation->setState(StationInfo::AppStarting);
        auto& apps = pStation->getStartAppNames();
        list<AppStartParameter> params;
        for (auto& app : apps)
        {
            params.push_back(AppStartParameter{ app.first.toStdString(),app.second.toStdString() });
        }
        try
        {
            prx->begin_startApp(params,
                [pStation](const AppStartingResults& results)
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
                    if (pStation->autoMonitorRemoteStartApp && (result.Result == AlreadyRunning || result.Result == Started))
                    {
                        //添加自动监视进程信息
                        pStation->addTempMonitorProcess(result.AppName, result.Id);
                    }
                }
                if (ok)
                {
                    pStation->setState(StationInfo::AppStarted, message);
                }
                else
                {
                    pStation->setState(StationInfo::AppStartFailure, message);
                }

                //设置新的监视列表
                try
                {
                    pStation->controlProxy->setWatchingApp(pStation->getAllMonitorProc());
                }
                catch (...)
                {
                    pStation->setState(StationInfo::GeneralError, QStringLiteral("设置工作站列表错误"));
                }
            },
                [pStation](const Ice::Exception& ex)
            {
                AppControlError *  controlError = dynamic_cast<AppControlError*>(const_cast<Ice::Exception*>(&ex));
                if (NULL != controlError)
                {
                    //应用程序启动代理没有启动
                    pStation->setState(StationInfo::AppStartFailure, QString::fromStdWString(controlError->Reason));
                    return;
                }
                Ice::ConnectionRefusedException *refusedError = dynamic_cast<Ice::ConnectionRefusedException*>(const_cast<Ice::Exception*>(&ex));
                if (NULL != refusedError)
                {
                    //网络故障,无法连接到工作站监控服务程序
                    pStation->setState(StationInfo::AppStartFailure, QStringLiteral("网络故障,无法连接到工作站监控服务程序"));
                    return;
                }
                //其他未知错误
                pStation->setState(StationInfo::AppStartFailure, QStringLiteral("未知错误"));
            });
        }
        catch (Ice::CommunicatorDestroyedException*)
        {
            pStation->setState(StationInfo::PowerOffOrNetworkFailure, QStringLiteral("连接通道已关闭"));
        }
        catch (IceUtil::IllegalArgumentException*)
        {
            pStation->setState(StationInfo::AppStartFailure, QStringLiteral("参数错误"));
        }
        catch (Ice::Exception* ex)
        {
            pStation->setState(StationInfo::AppStartFailure, ex->what());
        }
        catch (...)
        {
            pStation->setState(StationInfo::AppStartFailure, QStringLiteral("未知错误"));
        }
    }
}
