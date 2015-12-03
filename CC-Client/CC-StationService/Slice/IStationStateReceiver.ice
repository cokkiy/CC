#pragma once

[["cpp:include:list"]]
[["cpp:include:string"]]

//Center Control
module CC
{
	// 预定义控制接口
	interface IController;

	// string list, can't defines sequence<string> direct in struct
	//["cpp:type:std::list<std::string>"]
	["clr:generic:List"]	
	sequence<string> stringList;

	///定义了Network Interface information结构
	["cpp:type:std::map<std::string,std::list<std::string>>"]
	dictionary<string,stringList> NIInfo;

	//定义了工作站,主要是为了与其他工作站区别
	class Station
	{
		//工作站标识,为了区别与其他工作站而创建的工作站标识
		string stationId;
	};

	///工作站基本信息
	class StationSystemState extends Station
	{
		///总内存大小
		["cpp:type:long long"]
		long totalMemory;
		///计算机名称
		string computerName;
		///网卡信息,包括MAC地址和对应的IP地址,一个MAC地址可以有多个IP地址
		NIInfo NetworkInterfaces;
		///操作系统名称
		string osName;
		///操作系统版本
		string osVersion;
	};
	///工作站运行动态
	class StationRunningState extends Station
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
	class AppRunningState extends Station
	{
		///程序名称
		string appName;
		///是否在运行
		bool isRunning;
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
		///应用程序启动时间
		long startTime; 
	};

	//工作站状态接收接口
	interface IStationStateReceiver
	{
		///收集系统状态
		idempotent void receiveSystemState(StationSystemState systemState);
		///收集工作站运行动态
		idempotent void  receiveStationRunningState(StationRunningState stationRunningState, IController* controller);
		///收集程序运行状态
		idempotent void receiveAppRunningState(AppRunningState appRunningState);
	};
};