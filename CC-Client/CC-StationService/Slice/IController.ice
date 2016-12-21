#pragma once
#include "IStationStateReceiver.ice"
#include "AppControlParameter.ice"
#include "Version.ice"

//Center Control
module CC
{	   

	/// 应用程序控制异常
    exception AppControlError
	{
	    ///异常原因
		string Reason;
	};

	///文件信息
	["cpp:type:wstring"]
	struct FileInfo
	{
		//父路径
		string Parent;
		//路径名
		string Path;
		//是否是目录
		bool isDirectory;
		//创建时间
		string CreationTime;
		//最后修改时间
		string LastModify;
	};

	["clr:generic:List"]
	sequence<FileInfo> FileInfos;

	//目录访问错误
	["cpp:type:wstring"]
	exception DirectoryAccessError
	{
		string DirName;
		string Reason;
	};


	struct Path
	{
	   ["cpp:type:wstring"]
	   string Name;
	};

	//进程信息
	struct ProcessInfo
	{
		int Id;
		["cpp:type:wstring"]
		string Name;
		string Time;
		long MemorySize;
	}; 
	["clr:generic:List"]
	sequence<ProcessInfo> ProcessInfos;

	//指显软件显示控制命令
	enum DisplayCommand
	{
		FullScreen,
		RealtimeMode,
		ClearPage,
		NextPage,
		PrevPage,	
	};

	//气象云图下载选项
	struct WeatherPictureDowlnloadOption
	{
		// ftp 服务器地址
		string Url;
		// 用户名
		string UserName;
		// 密码
		string Password;
		// 下载间隔
		int Interval;
		// 下载最近几小时的图片
		int LastHours;
		//删除以前的文件
		bool DeletePreviousFiles;
		//删除多少小时前的
		int DeleteHowHoursAgo;
		//子文件夹
		["cpp:type:wstring"]
		string SubDirectory;
		// 保存位置(Linux)
		["cpp:type:wstring"]
		string SavePathForLinux;
		//保存位置(Windows)
		["cpp:type:wstring"]
		string SavePathForWindows;
	};
	

	///控制接口,定义了对工作站的控制工作
	interface IController
	{
		///启动指定的APP
		AppStartingResults startApp(AppStartParameters appParams) throws AppControlError;
		///关闭指定的应用程序
		AppControlResults closeApp(ProcessIdList processIdList)  throws AppControlError;
		///重启指定的应用程序
		AppStartingResults restartApp(AppStartParameters appParams)  throws AppControlError;

		//重启系统
		void reboot(bool force);
		///关闭系统
		void shutdown();

		///请求发送工作站基本信息
		idempotent StationSystemState  getSystemState(); 

		///设置要监视的进程（名称)列表
		idempotent void setWatchingApp(stringList procNames);

		///设置状态收集间隔（秒)
		idempotent void setStateGatheringInterval(int interval);	

		//设置气象图片下载选项
		idempotent void setWeatherPictureDownloadOption(WeatherPictureDowlnloadOption option);
		
		///捕获屏幕快照
		//*？ long position: 如果为0，则捕获新快照，否则，传送后续数据
		//*？ return：当返回false时，快照内容尚未读取完，返回true时，快照内容读取完毕
		bool captureScreen(long position,out int length,out ByteArray data) throws FileTransException;
		
		//获取文件夹下目录和文件信息
		//如果Path为空，则Linux下是/home，Windows下用户文档目录
		FileInfos getFileInfo(Path path) throws DirectoryAccessError;	

		//获取工作站全部进程信息
		ProcessInfos getAllProcessInfo();

		///获取服务器版本
		ServerVersion getServerVersion();

		//切换指显显示模式
		void switchDisplayPageAndMode(DisplayCommand cmd);

		//打开指定画面
		void openSpecPage(int pageNum);

		//获取中控服务安装绝对路径
		string getServicePath();
		//获取应用代理安装绝对路径
		string getLuncherProxyPath();
		//重命名文件
		bool renameFile(string oldName,string newName);
	};
	

	///定义文件传输接口
	interface IFileTranslation
	{
		///创建文件
		["cpp:type:wstring"]
		bool createFile(string fileName,string userName) throws FileTransException;
		///传送数据
		bool transData(long position,int length,ByteArray data) throws FileTransException;
		///关闭文件
		bool closeFile(string userName) throws FileTransException;

		///获取文件大小
		long getSize(string fileName) throws FileTransException;

		///获取文件数据
		/*
		* return：当返回false时，文件尚未读取完，返回true时，文件读取完毕
		*/
		["cpp:type:wstring"]
		bool getData(string fileName,long position,out int length,out ByteArray data) throws FileTransException;
	};
};