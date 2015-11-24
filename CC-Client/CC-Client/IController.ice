//Center Control
module CC
{
	//控制接口
	interface IController
	{
		//启动指定的APP
		void startApp(string appName);
		//重启系统
		void reboot(bool force);
		///关闭系统
		void shutdown();
	};
};