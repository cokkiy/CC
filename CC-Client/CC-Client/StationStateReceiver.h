#pragma once
#include "IStationStateReceiver.h"
#include <Ice/Ice.h>
#include "StationList.h"
class UpdateManager;
//////////////////////////////////////////////////////////////////////////
//
// \class StationStateReceiver 实现工作站状态接收接口\see CC::IStationStateReceiver
// 接收工作站基本信息,运行信息和应用程序运行状态
// 
//////////////////////////////////////////////////////////////////////////
class StationStateReceiver : public CC::IStationStateReceiver
{
private:
    // 工作站列表
    StationList* pStations;
    //是否自动更新工作站列表
    bool autoRefreshStationList;
	//更新管理器
	UpdateManager* updateManager;
private:
    //查找并更新工作站信息
    StationInfo* findAndSetStationSystemState(const ::CC::StationSystemState& pStationSystemState);
public:
    /*!
    创建一个工作站信息接收对象
	@param UpdateManager* updateManager 更新管理器
    @param StationList* pStations 工作站列表
    @param bool autoRefreshStationList 是否自动更新工作站列表
    @return 工作站信息接收对象
    作者：cokkiy（张立民)
    创建时间：2015/11/23 21:16:50
    */
    StationStateReceiver(UpdateManager* updateManager, StationList* pStations, bool autoRefreshStationList = true);
    /*释放资源*/
    ~StationStateReceiver() = default;

    /*!
    接收工作站系统状态
    @param const ::CC::StationSystemStatePtr & 接收到的工作站系统状态
    @param const ::Ice::Current & 
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/24 9:38:28
    */
    virtual void receiveSystemState(const ::CC::StationSystemState&, const ::Ice::Current& = ::Ice::Current()) override;

    /*!
    接收工作站运行状态
    @param const ::CC::StationRunningStatePtr & 接收到工作站运行状态
    @param const ::Ice::Current & 
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/24 9:39:14
    */
    virtual void receiveStationRunningState(const ::CC::StationRunningState&, const ::CC::IControllerPrx&, const ::CC::IFileTranslationPrx&, const ::Ice::Current& = ::Ice::Current()) override;

    /*!
    接收工作站应用程序状态
    @param const ::CC::AppRunningStatePtr & 接收到的工作站应用程序状态
    @param const ::Ice::Current & 
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/24 9:39:34
    */
    virtual void receiveAppRunningState(const ::CC::AppsRunningState&, const ::Ice::Current& = ::Ice::Current()) override;

    /*!
    设置工作站列表
    @param StationList * pStations 工作站列表指针
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/24 9:28:31
    */
    void setStations(StationList* pStations);

    /*!
    设置是否自动更新工作站列表
    @param bool autoRefresh 是否自动更新工作站列表
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/01 18:02:53
    */
    void setAutoRefreshStationList(bool autoRefresh);
};

