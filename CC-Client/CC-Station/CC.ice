module CC
{
	exception Error{};
	interface Controller
	{		
		// 启动指定的APP
		void startApp(string appName) throws Error;
		//重启系统
		void reboot(bool force);
	};
};