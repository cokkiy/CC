#pragma once

#include "Version.ice"
[["cpp:include:list"]]
[["cpp:include:string"]]

//Center Control
module CC
{
	// 预定义控制接口
	interface IController;
	interface IFileTranslation; 

	// string list, can't defines sequence<string> direct in struct
	["cpp:type:std::list<std::string>"]
	sequence<string> stringList;

	///定义了Network Interface information结构
	["cpp:type:std::map<std::string,std::list<std::string>>"]
	dictionary<string,stringList> NIInfo;

	///工作站基本信息
	struct StationSystemState
	{
		//工作站标识,为了区别与其他工作站而创建的工作站标识
		string stationId;
		///总内存大小
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
	struct StationRunningState
	{
		//工作站标识,为了区别与其他工作站而创建的工作站标识
		string stationId;
		///系统当前物理内存总占用字节数
		long currentMemory;
		//系统cpu总占用率
		float cpu;
		///系统进程数量
		int procCount;
		///中控服务和代理版本信息
		ServerVersion Version;
	};

	///进程信息
	struct Process
	{
		///进程ID
		int Id;
		///进程名称
		string ProcessName;
		///进程监控名称,对监控列表中的进程,该名称就等于列表中的名称,也等于进程名（如果进程存在),
		///对远程启动自动监控的进程来说,该名称等于Id:xx,xx表示进程Id,如果程序没有成功启动,则等于-1
		string ProcessMonitorName;
	};

	["cpp:type:std::list<Process>"]
	sequence<Process> Processes;

	///程序运行状态
	struct AppRunningState
	{
		//工作站标识,为了区别与其他工作站而创建的工作站标识
		string stationId;
		///进程信息
		Process Process;
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

	//运行程序状态列表
	["cpp:type:std::list<AppRunningState>"]
	sequence<AppRunningState> AppsRunningState;

	//工作站状态接收接口
	interface IStationStateReceiver
	{
		///收集系统状态
		idempotent void receiveSystemState(StationSystemState systemState);
		///收集工作站运行动态
		idempotent void  receiveStationRunningState(StationRunningState stationRunningState, IController* controller,IFileTranslation* fileProxy );
		///收集程序运行状态
		idempotent void receiveAppRunningState(AppsRunningState appsRunningState);
	};
};