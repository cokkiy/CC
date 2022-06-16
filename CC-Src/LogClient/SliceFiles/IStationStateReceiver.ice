#pragma once

#include "StationState.ice"
#include "IController.ice"


//Center Control
module CC
{
	interface IController;
	interface IFileTranslation;
	//工作站状态接收接口
	interface IStationStateReceiver
	{
		///收集系统状态
		idempotent void receiveSystemState(StationSystemState systemState);
		///收集工作站运行动态
		idempotent void  receiveStationRunningState(StationRunningState stationRunningState, IController* controller, IFileTranslation* fileProxy );
		///收集程序运行状态
		idempotent void receiveAppRunningState(AppsRunningState appRunningState);
		//收集网络状态
		idempotent void receiveNetStatistics(string data);	
	};
};