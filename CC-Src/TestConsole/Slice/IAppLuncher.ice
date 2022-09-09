#pragma once
#include "AppControlParameter.ice"
module AppController
{
	interface ILuncher
	{
		///启动指定的APP
		CC::AppStartingResults startApp(CC::AppStartParameters appParams);
		///关闭指定的应用程序
		CC::AppControlResults closeApp(CC::ProcessIdList processIdList);
		///重启指定的应用程序
		CC::AppStartingResults restartApp(CC::AppStartParameters appParams);
	};
};