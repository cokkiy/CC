[["cpp:include:list"]]
[["cpp:include:string"]]
//Center Control
module CC
{

	// string list, can't defines sequence<string> direct in struct
	["cpp:type:std::list<std::string>"]	
	sequence<string> stringList;

	///工作站基本信息
	struct StationSystemState
	{
		///总内存大小
		["cpp:type:long long"]
		long totalMemory;
		///计算机名称
		string computerName;
		///IP地址
		["cpp:type:std::list<std::string>"]		
		stringList IP;
		///MAC地址
		["cpp:type:std::list<std::string>"]
		stringList mac;
		///操作系统名称
		string osName;
		///操作系统版本
		string osVersion;
	};
	///工作站运行动态
	struct StationRunningState
	{
		///系统当前物理内存总占用字节数
		["cpp:type:long long"]
		long currentMemory;
		//系统cpu总占用率
		float cpu;
		///系统进程数量
		int procCount;
	};

	///程序运行状态
	struct AppRunningState
	{
		///程序名称
		string appName;
		///cpu占用率
		float cpu;
		///进程数
		int procCount;
		///线程数
		int threadCount;
		///当前物理内存占用字节数
		["cpp:type:long long"]
		long currentMemory;
		///程序版本
		string appVersion;
	};

	//工作站状态接收接口
	interface IStationStateReceiver
	{
		///收集系统状态
		idempotent StationSystemState receiveSystemState();
		///收集工作站运行动态
		idempotent StationRunningState receiveStationRunningState();
		///收集程序运行状态
		idempotent AppRunningState receiveAppRunningState();
	};
};