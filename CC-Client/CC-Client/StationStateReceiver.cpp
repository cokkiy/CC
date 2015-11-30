#include "StationStateReceiver.h"
#include "StationList.h"


/*!
创建一个工作站信息接收对象
@param StationList* pStations 工作站列表
@return 工作站信息接收对象
作者：cokkiy（张立民)
创建时间：2015/11/23 21:16:50
*/
StationStateReceiver::StationStateReceiver(StationList* pStations)
{
    this->pStations = pStations;
}

/*释放资源*/
StationStateReceiver::~StationStateReceiver()
{
}

/*!
接收工作站系统状态
@param const ::CC::StationSystemState & 接收到的工作站系统状态
@param const ::Ice::Current &
@return void
作者：cokkiy（张立民)
创建时间：2015/11/24 9:38:28
*/
void StationStateReceiver::receiveSystemState(const ::CC::StationSystemStatePtr& pStationSystemState, const ::Ice::Current& /*= ::Ice::Current()*/)
{
    if (pStations != nullptr)
    {
        for (auto& ni : pStationSystemState->NI)
        {
            StationInfo* pStation = pStations->find(ni);
            if (pStation != NULL)
            {
                pStation->setStationId(pStationSystemState->stationId);
                pStation->setTotalMemory(pStationSystemState->totalMemory);
                pStation->setOSName(pStationSystemState->osName);
                pStation->setOSVersion(pStationSystemState->osVersion);
                pStation->setLastTick();
                pStation->setState(StationInfo::Warning); //警告：程序没有启动
                pStation->setHint(QStringLiteral("应用程序没有启动"));
            }
        }
    }
}

/*!
接收工作站运行状态
@param const ::CC::StationRunningState & 接收到工作站运行状态
@param const ::Ice::Current &
@return void
作者：cokkiy（张立民)
创建时间：2015/11/24 9:39:14
*/
void StationStateReceiver::receiveStationRunningState(const ::CC::StationRunningStatePtr& pStationRunningState, const ::Ice::Current& /*= ::Ice::Current()*/)
{
    if (pStations != nullptr)
    {
        StationInfo* pStation = pStations->findById(pStationRunningState->stationId);
        if (pStation != NULL)
        {
            pStation->setCPU(pStationRunningState->cpu);
            pStation->setMemory(pStationRunningState->currentMemory);
            pStation->setProcCount(pStationRunningState->procCount);
            pStation->setLastTick();
        }
    }
}

/*!
接收工作站应用程序状态
@param const ::CC::AppRunningState & 接收到的工作站应用程序状态
@param const ::Ice::Current &
@return void
作者：cokkiy（张立民)
创建时间：2015/11/24 9:39:34
*/
void StationStateReceiver::receiveAppRunningState(const ::CC::AppRunningStatePtr& pAppRunningState, const ::Ice::Current& /*= ::Ice::Current()*/)
{
    if (pStations != nullptr)
    {
        StationInfo* pStation = pStations->findById(pAppRunningState->stationId);
        if (pStation != NULL)
        {
            pStation->setAppCPU(pAppRunningState->cpu);
            pStation->setAppMemory(pAppRunningState->currentMemory);
            pStation->setAppThreadCount(pAppRunningState->threadCount);
            pStation->setAppIsRunning(true);
            pStation->setState(StationInfo::Normal);
            pStation->setLastTick();
        }
    }
}

/*!
设置工作站列表
@param StationList * pStations 工作站列表指针
@return void
作者：cokkiy（张立民)
创建时间：2015/11/24 9:28:31
*/
void StationStateReceiver::setStations(StationList* pStations)
{
    this->pStations = pStations;
}
