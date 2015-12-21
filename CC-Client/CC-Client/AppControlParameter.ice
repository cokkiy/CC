#pragma once

[["cpp:include:list"]]

//Center Control
module CC
{
	["cpp:type:std::list<int>"]	
	sequence<int> ProcessIdList;	

	///程序控制结果
	enum AppControlResult {AlreadyRunning, Started, FailToStart,NotRunnning,Closed, FailToClose, Error};

	///进程信息
	struct AppStartingResult
	{
		///进程ID
		int Id;
		///进程名称
		string ProcessName;
		///返回的应用程序名称（包括路径和启动参数),路径与启动参数之间用 && 连接
		string AppName;
		///控制结果
		AppControlResult Result;
	};

	///应用程序启动参数
	struct AppStartParameter
	{
		///程序路径
		string AppPath;
		///程序参数
		string Arguments;
	};

	["cpp:type:std::list<CC::AppStartingResult>"]
	sequence<CC::AppStartingResult> AppStartingResults;	
		
	["cpp:type:std::list<CC::AppControlResult>"]
	sequence<CC::AppControlResult> AppControlResults;	

	["cpp:type:std::list<CC::AppStartParameter>"]
	sequence<CC::AppStartParameter> AppStartParameters;	
};