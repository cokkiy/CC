// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：datastruct.h
// 文件摘要：声明综合应用分系统公用数据结构。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef DATASTRUCT_H
#define DATASTRUCT_H 

#include<iostream>
#include<string>
#include<string.h>
#include<map>
#include<set>
#include<list>
#include<bitset>
#include<vector>

#include<sys/types.h>

#include "const.h"

using namespace std ;

//struct ProcessStatus 				//进程状态
//{
//	u_int8_t processNo; 						//子进程序号 1:网络收发进程|2:双工管理进程|3:信息处理进程|4:辅助决策进程|5:应急进程
//	string processName; 						//子进程名串
//	pid_t pId;      								//子进程ID
//	u_int8_t pStatus;   						//子进程状态 0:停止|1:运行|2:挂起
//};

//struct InstructionSet 						//指令集(增补指令)
//{
//	u_int16_t tableNo;							//指令参数表号
//	u_int16_t paramNo;							//指令参数号
//	string remark;   								//备注
//	u_int32_t value;								//指令参数值
//};

typedef unsigned int u_int32_t;
typedef unsigned short u_int16_t;
typedef unsigned char u_int8_t;

struct LogRecord						//日志记盘结构
{
	u_int32_t logNo;								//日志序号
	u_int32_t logTime;						//日志发生时间
	u_int32_t logType;						//日志类型 1 信息; 2 错误
	char logSrc[20];						//日志来源
	char logContent[200];				//日志内容
};

struct ParamRecord					//参数(包括指挥命令与辅助决策结果)记盘结构
{
	u_int32_t paramTime;					//参数产生时间,子帧时间
	u_int16_t tableNo;							//参数表号
	u_int16_t paramNo;							//参数号
	u_int16_t paramVLen;						//参数值长度
	char paramName[MAX_PARAM_NAME_LEN]; 				//参数名串
	char dataType[10];							//参数值数据类型
	char valueStr[MAX_PARAMETER_LEN];					//参数值串
};

struct tm_s										//在linux c中时间结构的基础上增加了毫秒
{
	struct tm tms;
	int tms_msec;   						//毫秒
};

struct BackComStatus     			//回令
{
	unsigned short m_tabNo;
	unsigned short m_paramNo;
	unsigned char  m_paramVal;
};

struct structDecisionaidNode
{
	string name;
	string ipAddr;
	unsigned short port;
};

class NetParameter
{
	public:
		string          m_netAddr;
  		string          m_nodeName;
		u_int8_t   		m_deviceCode;
		u_int16_t       m_netPort;
		
		NetParameter(): m_netAddr(""),m_nodeName(""), m_deviceCode(0), m_netPort(0){}
		NetParameter( string addr, string nname, u_int8_t dcode, u_int16_t port)
	              : m_netAddr(addr), m_nodeName(nname), m_deviceCode(dcode), m_netPort(port){}
};

//struct NetWatchData
//{
//	unsigned int recvFrameNum;                       //接收帧计数
//	unsigned int recvByteNum;												 //接收字节计数
//	unsigned int sendFrameNum;                       //发送帧计数
//	unsigned int sendByteNum;												 //发送字节计数
//};

struct stru_DataDispenseCfg
{
	//界面显示配置信息
	bool		MainOrSub ;		//软双工本机默认主副机标志  服务器1为主 服务器2为副
	u_int16_t	RefreshCyc;		//界面刷新周期	（200ms-1000ms）
	u_int16_t	MaxLogNum;		//日志显示最大条数（50-500）
	u_int16_t	MaxLogLen;		//日志缓冲区队列最大长度（50-100）
	char        RecDir[100];
	u_int8_t	SrcSel[255];	//界面显示详细帧计数中的选择的信源	
	
	//信息处理配置信息
	u_int8_t	InfoType;		//帧头帧类别
	u_int8_t	Src;			//本机信源
	u_int8_t	TaskCode;		//任务代号
	u_int8_t	HLCode;			//合练代号
	u_int16_t	dfzxTOTabNo;	//东风中心T0表号
	u_int16_t	DfzxTOParamNo;	//东风中心T0参数号
	u_int16_t	C3iT0TabNo;		//C3I系统T0表号
	u_int16_t	C3iT0ParamNo;	//C3I系统T0参数号
	u_int8_t	Thread0ProcSrc[50];	//第0线程处理信源设备代号
	u_int8_t	Thread1ProcSrc[50];	//第1线程处理信源设备代号
	u_int8_t	Thread2ProcSrc[50];	//第2线程处理信源设备代号
	u_int8_t	Thread3ProcSrc[50];	//第3线程处理信源设备代号
	u_int8_t	Thread4ProcSrc[50];	//第4线程处理信源设备代号
	u_int8_t	Thread5ProcSrc[50];	//第5线程处理信源设备代号

	//软双工信息设置
	u_int32_t   DuplexLocalAddr;	//软双工本机地址
	u_int16_t   LocalPort;			//软双工本机端口号
	u_int32_t   DuplexOtherAddr;	//软双工他机地址
	u_int16_t   OtherPort;			//软双工他机端口号
	
	//网络设置
	//u_int16_t   C3IPort;			//C3I系统使用的端口号
	//u_int8_t	GroupCode;			//本机所在网络组
	//u_int32_t   ComAidAddr[10];			//辅助决策进程所使用的地址
	//u_int16_t   ComAidPort[10];			//辅助决策进程使用的端口号

};

struct stru_SoftLockInfo
{
	char aPwdSav[145]; //密码内容
	char cLockFlag;    //软件加锁标志 见const.h
	char cRemPwdFlag;  //记住密码标志
	char cPwdLen;	   //密码长度
//	char aValidFlag;   //密码数据有效标志
};

typedef std::list<unsigned char> OutNode;
typedef std::vector<NetParameter> NetworkSet;
//typedef map<unsigned short, struct NetWatchData> NetWatchDataMap;



#endif
