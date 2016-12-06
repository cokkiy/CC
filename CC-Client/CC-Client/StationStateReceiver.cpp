#include "StationStateReceiver.h"
#include "StationList.h"
#include "IController.h"
#include "option.h"
#include "updatemanager.h"
using namespace std;


/*!
创建一个工作站信息接收对象
@param StationList* pStations 工作站列表
@return 工作站信息接收对象
作者：cokkiy（张立民)
创建时间：2015/11/23 21:16:50
*/
StationStateReceiver::StationStateReceiver(UpdateManager* updateManager, StationList* pStations, bool autoRefreshStationList)
{
    this->pStations = pStations;
    this->autoRefreshStationList = autoRefreshStationList;
	this->updateManager = updateManager;
}


/*!
接收工作站系统状态
@param const ::CC::StationSystemState & 接收到的工作站系统状态
@param const ::Ice::Current &
@return void
作者：cokkiy（张立民)
创建时间：2015/11/24 9:38:28
*/
void StationStateReceiver::receiveSystemState(const ::CC::StationSystemState& pStationSystemState, const ::Ice::Current& /*= ::Ice::Current()*/)
{
    findAndSetStationSystemState(pStationSystemState);
}

/*!
接收工作站运行状态
@param const ::CC::StationRunningState & 接收到工作站运行状态
@param const ::Ice::Current &
@return void
作者：cokkiy（张立民)
创建时间：2015/11/24 9:39:14
*/
void StationStateReceiver::receiveStationRunningState(const ::CC::StationRunningState& stationRunningState, const ::CC::IControllerPrx& controlPrx,
	const ::CC::IFileTranslationPrx& fileProxy, const ::Ice::Current& /*= ::Ice::Current()*/)
{
    if (pStations != nullptr)
    {
        StationInfo* pStation = pStations->findById(stationRunningState.stationId);
        if (pStation != NULL)
        {
            pStation->setCPU(stationRunningState.cpu);
            pStation->setMemory(stationRunningState.currentMemory);
            pStation->setProcCount(stationRunningState.procCount);
            pStation->setLastTick();
            if (stationRunningState.Version.AppLuncherVersion.Major != 0)
			{
                updateManager->addStationVersion(pStation, stationRunningState.Version);
			}
        }
        else
        {
            //没有收到工作站基本信息,请求发送基本信息
            controlPrx->begin_getSystemState(
				[this,controlPrx,fileProxy,&stationRunningState](const CC::StationSystemState& systemState) 
			{
                StationInfo* pStation = findAndSetStationSystemState(systemState);
				if (pStation != NULL)
				{
					//设置控制代理
					pStation->controlProxy = controlPrx;
					pStation->fileProxy = fileProxy;
					//设置运行状态
					pStation->setCPU(stationRunningState.cpu);
					pStation->setMemory(stationRunningState.currentMemory);
					pStation->setProcCount(stationRunningState.procCount);
					pStation->setLastTick();
					//设置监视间隔
					controlPrx->begin_setStateGatheringInterval(Option::getInstance()->Interval, []() {});
					//设置工作站监视进程列表
					controlPrx->begin_setWatchingApp(pStation->getAllShouldMonitoredProcessesName(), []() {});
					CC::WeatherPictureDowlnloadOption ccOption;
					Option* option = Option::getInstance();
					ccOption.Url = option->weatherImageDownloadOption.Url.toStdString();
					ccOption.UserName = option->weatherImageDownloadOption.UserName.toStdString();
					ccOption.Password = option->weatherImageDownloadOption.Password.toStdString();
					ccOption.LastHours = option->weatherImageDownloadOption.LastHours;
					ccOption.Interval = option->weatherImageDownloadOption.Interval;
					ccOption.SavePathForWindows = option->weatherImageDownloadOption.SavePathForWindows.toStdWString();
					ccOption.SavePathForLinux = option->weatherImageDownloadOption.SavePathForLinux.toStdWString();
					controlPrx->begin_setWeatherPictureDownloadOption(ccOption, []() {});
				}
            },
				[](const Ice::Exception& ex)
			{
			});			
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
void StationStateReceiver::receiveAppRunningState(const ::CC::AppsRunningState& appsRunningState, const ::Ice::Current& /*= ::Ice::Current()*/)
{
    if (pStations != nullptr&&!appsRunningState.empty())
    {
        StationInfo* pStation = pStations->findById(appsRunningState.front().stationId);
        if (pStation != NULL)
        {
            pStation->setAppsRunningState(appsRunningState);
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

//查找并更新工作站信息
StationInfo* StationStateReceiver::findAndSetStationSystemState(const ::CC::StationSystemState& stationSystemState)
{
    StationInfo* pStation = NULL;
    if (pStations != nullptr)
    {
        for (auto& ni : stationSystemState.NetworkInterfaces)
        {
            pStation = pStations->find(ni);
            if (pStation != NULL)
            {
                break;
            }
        }
        if (pStation == NULL&&autoRefreshStationList)
        {
            StationInfo station;
            station.setStationId(stationSystemState.stationId);
            station.setName(QString::fromStdString(stationSystemState.computerName));
            for (auto& ccNI : stationSystemState.NetworkInterfaces)
            {
                NetworkInterface ni(ccNI);
                station.NetworkIntefaces.push_back(ni);
            }
            station.addStandAloneMonitorProcesses(Option::getInstance()->MonitorProcesses);
            station.addStartApps(Option::getInstance()->StartApps);
            pStations->push(station);
            pStation = pStations->findById(stationSystemState.stationId);
        }

        if (pStation != NULL)
        {
            pStation->setStationId(stationSystemState.stationId);
            pStation->setTotalMemory(stationSystemState.totalMemory);
            pStation->setOSName(stationSystemState.osName);
            pStation->setOSVersion(stationSystemState.osVersion);
            pStation->setLastTick();
        }
    }
    return pStation;
}

/*!
设置是否自动更新工作站列表
@param bool autoRefresh 是否自动更新工作站列表
@return void
作者：cokkiy（张立民)
创建时间：2015/12/01 18:02:53
*/
void StationStateReceiver::setAutoRefreshStationList(bool autoRefresh)
{
    this->autoRefreshStationList = autoRefresh;
}
