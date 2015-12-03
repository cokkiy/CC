/*****************************************************************************
 工作站@class StationManager类实现,实现了开机,关机,重启,启动程序,关闭程序
 等动作
 *****************************************************************************/
#include "stationmanager.h"
#include "IController.h"
#include <Ice/Ice.h>
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
StationManager::StationManager(StationList* stations, Ice::CommunicatorPtr  communicator,
    const QModelIndexList& indexs /*= QModelIndexList()*/, QObject *parent /*= NULL*/)
    :QObject(parent)
{
    this->pStations = stations;
    this->stationIndexs = indexs;
    this->communicator = communicator;
}

/*!
全部开机
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:53:18
*/
void StationManager::powerOnAll()
{

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
        reboot();
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
//     auto controller = CCApplication::getController();
//     if (controller)
//     {
//         controller->reboot(false);
//     }
}




/*!
关闭选择的计算机
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:54:00
*/
void StationManager::shutdown()
{

}
/*!
重启全部的计算机
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:54:12
*/
void StationManager::rebootAll()
{

}
/*!
关闭全部的计算机
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:54:29
*/
void StationManager::shutdownAll()
{

}
/*!
关闭程序（指显程序)
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:54:41
*/
void StationManager::closeApp()
{

}
/*!
重启程序（指显程序)
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:55:05
*/
void StationManager::restartApp()
{

}
/*!
退出程序（指显程序)
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:55:19
*/
void StationManager::exitApp()
{

}
/*!
退出全部工作站指显程序
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:58:22
*/
void StationManager::exitAllApp()
{

}
/*!
启动程序
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:55:34
*/
void StationManager::startApp()
{
//     auto controller = CCApplication::getController();
//     if (controller)
//     {
//         controller->startApp("vscode");
//     }
}



/*!
启动所有工作站指显程序
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 15:57:46
*/
void StationManager::startAllApp()
{

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
@param int interval 监视间隔（毫秒)
@return void
作者：cokkiy（张立民)
创建时间：2015/12/01 11:42:42
*/
void StationManager::setInterval(int interval)
{
//     int port = communicator->getProperties()->getPropertyAsInt("Controller.Port");
//     for (auto& station : *pStations)
//     {
//         QString proxyString = QStringLiteral("control:tcp -h %1 -p %2").arg(station.IP).arg(port);
//         auto proxy = communicator->stringToProxy(proxyString.toStdString());
//         IControllerPrx prx = IControllerPrx::uncheckedCast(proxy);
//         prx->begin_setStateGatheringInterval(interval,
//             [](const ::Ice::AsyncResultPtr& result)
//         {
//             bool c = result.get()->isCompleted();
//         });
//     }
}


