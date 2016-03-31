#pragma once
#include "IStationStateReceiver.ice"
#include "AppControlParameter.ice"

//Center Control
module CC
{	

	/// 应用程序控制异常
    exception AppControlError
	{
	    ///异常原因
		string Reason;
	};

	///控制接口,定义了对工作站的控制工作
	interface IController
	{
		///启动指定的APP
		AppStartingResults startApp(AppStartParameters appParams) throws AppControlError;
		///关闭指定的应用程序
		AppControlResults closeApp(ProcessIdList processIdList)  throws AppControlError;
		///重启指定的应用程序
		AppStartingResults restartApp(AppStartParameters appParams)  throws AppControlError;

		//重启系统
		void reboot(bool force);
		///关闭系统
		void shutdown();

		///请求发送工作站基本信息
		idempotent StationSystemState  getSystemState(); 

		///设置要监视的进程（名称)列表
		idempotent void setWatchingApp(stringList procNames);

		///设置状态收集间隔（毫秒)
		idempotent void setStateGatheringInterval(int interval);
	};
};