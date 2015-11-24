module CCService
{
	exception Error{};
	//Interface for controller
	interface IController
	{		
		// 启动指定的APP
		void startApp(string appName) throws Error;
		//重启系统
		void reboot(bool force);
		//关闭系统
		void shutdown();
	};	
};