#pragma once

[["cpp:include:list"]]

//Center Control
module CC
{
	["cpp:type:std::list<int>"]	
	sequence<int> ProcessIdList;	

	///字节数组
	["cpp:array"]
	sequence<byte> ByteArray;


	///程序控制结果
	enum AppControlResult {AlreadyRunning, Started, FailToStart,NotRunnning,Closed, FailToClose, Error};

	///进程信息
	struct AppStartingResult
	{
		///应用程序启动参数标识
		int ParamId;
		///进程ID
		int ProcessId;
		///进程名称
		string ProcessName;		
		///控制结果
		AppControlResult CtrlResult;
		///结果说明
		["cpp:type:wstring"]
		string Result;
	};

	///应用程序启动参数
	struct AppStartParameter
	{
		///参数标识,用于返回启动结果
		int ParamId;
		///程序路径
		string AppPath;
		///程序参数
		string Arguments;
		///进程名
		string ProcessName;
		///是否允许允许多个实例
		bool AllowMultiInstance=false;
	};

	///文件传输异常
	exception FileTransException
	{
		["cpp:type:wstring"]
		string fileName;
		long offset;
		int length;
		int dataLen;
		["cpp:type:wstring"]
		string Message;
	};

	["cpp:type:std::list<CC::AppStartingResult>"]
	sequence<CC::AppStartingResult> AppStartingResults;	
		
	["cpp:type:std::list<CC::AppControlResult>"]
	sequence<CC::AppControlResult> AppControlResults;	

	["cpp:type:std::list<CC::AppStartParameter>"]
	sequence<CC::AppStartParameter> AppStartParameters;	
};