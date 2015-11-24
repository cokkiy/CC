#pragma once
#include "IStationStateReceiver.h"
#include <Ice/Ice.h>
//////////////////////////////////////////////////////////////////////////
//
// \class StationStateReceiver 实现工作站状态接收接口\see CC::IStationStateReceiver
// 
//////////////////////////////////////////////////////////////////////////
class StationStateReceiver :CC::IStationStateReceiver
{
public:
    /*!
    创建一个工作站信息接收对象
    @return 工作站信息接收对象
    作者：cokkiy（张立民)
    创建时间：2015/11/23 21:16:50
    */
    StationStateReceiver();
    /*释放资源*/
    ~StationStateReceiver();

    /*!
    接收工作站系统状态
    @param const ::Ice::Current & 
    @return CC::StationSystemState 接收到的工作站系统状态
    作者：cokkiy（张立民)
    创建时间：2015/11/23 21:14:34
    */
    virtual CC::StationSystemState receiveSystemState(const ::Ice::Current& = ::Ice::Current()) override;

    /*!
    接收工作站运行状态
    @param const ::Ice::Current & 
    @return CC::StationRunningState 接收到工作站运行状态
    作者：cokkiy（张立民)
    创建时间：2015/11/23 21:15:04
    */
    virtual CC::StationRunningState receiveStationRunningState(const ::Ice::Current& = ::Ice::Current()) override;

    /*!
    接收工作站应用程序状态
    @param const ::Ice::Current & 
    @return CC::AppRunningState 接收到的工作站应用程序状态
    作者：cokkiy（张立民)
    创建时间：2015/11/23 21:15:31
    */
    virtual CC::AppRunningState receiveAppRunningState(const ::Ice::Current& = ::Ice::Current()) override;

};

