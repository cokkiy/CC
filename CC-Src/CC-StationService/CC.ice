module CC
{
	interface Controller
	{
		exception Error{};
		//启动指定的APP
		idempotent void startApp(string appName)
		throw Error;
		//重启系统
		void reboot(bool force);
	};
};