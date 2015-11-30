#pragma once

// 引用系统状态
#include "IStationStateReceiver.ice"
//Center Control
module CC
{

	///控制接口,定义了对工作站的控制工作
	interface IController
	{
		///启动指定的APP
		void startApp(string appName);
		///关闭指定的应用程序
		void closeApp(string appName);
		///重启指定的应用程序
		void restartApp(string appName);

		//重启系统
		void reboot(bool force);
		///关闭系统
		void shutdown();

		///获取工作站系统状态
		StationSystemState getSystemState(); 

		///设置要监视的进程（名称)列表
		void setWatchingApp(stringList procNames);

		///设置状态收集间隔（毫秒)
		void setStateGatheringInterval(int interval);
	};
};