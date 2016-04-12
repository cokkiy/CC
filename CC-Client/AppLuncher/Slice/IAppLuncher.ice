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

		///捕获屏幕快照
		//*？ long position: 如果为0，则捕获新快照，否则，传送后续数据
		//*？ return：当返回false时，快照内容尚未读取完，返回true时，快照内容读取完毕
		bool captureScreen(long position,out int length,out CC::ByteArray data) throws CC::FileTransException;
	};
};