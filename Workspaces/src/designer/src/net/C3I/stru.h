#pragma once
#pragma warning( disable: 4786 )  /* more than 64k source lines */
#pragma warning( disable: 4800 )  /* more than 64k source lines */
#pragma warning( disable: 4101 )  /* more than 64k source lines */

#include <map>
#include <string>
#include <vector>
#include <QTime>
//#include "CElementCom.h"
//#include "CElementList.h"
#include "../abstractparam.h"
//定义DWORD
typedef unsigned long DWORD;
typedef unsigned short WORD;

#define Pi				3.1415926535897932384626433832795
#define CONST_CURVEBUFFER  8*36000
#define  MeansValueMap std::map< long, DWORD >

class CElementList;
//参数解释的结构，包含文本和颜色，及是否设置颜色等属性
typedef struct stru_MeanValue
{
	char * content;
	//QColor color;
	bool definecolor;
	stru_MeanValue():content(NULL),definecolor(false){}
}MeanValue;
//日期结构
struct stru_C3IDate
{
	unsigned	Day			: 4;//日
	unsigned	Day_10		: 4;//十日
	unsigned	Month		: 4;//月
	unsigned	Month_10	: 4;//十月
	unsigned 	Year		: 4;//年
	unsigned 	Year_10		: 4;//十年
	unsigned 	Year_100	: 4;//百年
	unsigned 	Year_1000	: 4;//千年
};
//鼠标提示信息结构
struct stru_ToolTipInfo
{
	//包含信息内容的文本阵列
	QStringList nControlInfo;
	
	//文本颜色
	//QColor nTextColor;

	//背景颜色
    //QColor nBackColor;
};
//
struct stru_ParamColor 
{
	double v0;
	double v1;
    //QColor color;
	/********************************************************************/
	//	修改标志：		zyxh
	//	修改日期时间:	2012:6:4   8:49:32
	//	修改人:			谢虎

	//变色开始时间[日期、时间]，当全F时表示无
	unsigned long ErrStarDate;
	unsigned long ErrStarTime;
	
	//	修改标志：		zyxh
	/********************************************************************/
};
//
struct stru_ParamInfoTemplet 
{
	char* pParam[7];
	stru_ParamInfoTemplet()
	{
		for (int i=0;i<7;i++)
		{
			pParam[i] = NULL;
		}
	}
};

struct stru_Remark
{
	bool bYCCS;//是否为遥测参数
	unsigned long YCCSInvalidValue;//遥测参数无效值
	bool bFYJD;//是否为俯仰角度值
	bool bFWJD;//是否为方位角度值
	unsigned char FYJDlb;//方位角度值类别，1为-180°~ 180°，2为0°~ 360°
	bool bSJCS;//是否为时间参数
	unsigned char SJCSlb;//时间转换结果类别，1为相对时，2为绝对时
	unsigned char T0Number;//T0编号
	bool bYMXS;//是否显示源码
}; 

//C3I参数与参数含义组的对应关系
struct stru_ParamIndex
{
	unsigned long ulGroupIndex;//组号
    AbstractParam* pParam;//对应C3I参数
};

//C3I系统网络设备信息结构体
struct stru_C3INetInfo 
{
public:
	//设备名称
	char sDevName[64];

	//设备代号
	unsigned short usDevCode;

	//设备输入IP地址1
	char sDevIntIP1[16];

	//设备输入IP地址2
	char sDevIntIP2[16];

	//设备输出IP地址
	char sDevExtIP[16];

	//设备的计算机类别码
	char sComputerCode[16];
};

//错误日志存储结构体
struct stru_ErrorLog 
{
public:
	//错误发生的时间。
	//typedef struct _SYSTEMTIME {  // st 
    //WORD wYear; 
    //WORD wMonth; 
    //WORD wDayOfWeek; 
    //WORD wDay; 
    //WORD wHour; 
    //WORD wMinute; 
    //WORD wSecond; 
    //WORD wMilliseconds; 
	//} SYSTEMTIME;
	QTime Time;

	//错误的详细记录。
	char* pcErrorDescription;

};

//记盘配置存储结构体
struct stru_FileSaveConfig 
{
public:
	//记盘文件路径
	char sSaveFilePath[255];

	//记盘文件名称
	char sSaveFileName[64];

	//记盘文件的大小，以KB为单位
	unsigned short usSaveFileLength;

};

//历史曲线存储结构体，其中包含了参数的时间特性和参数信息。
struct stru_HistoryCurve 
{
public:
	//历史曲线起始时间。该数值是一个整形，含义为从1970年1月1日
	//上午8点到错误发生时间所经过的秒数。
	long lBeginTime;

	//历史曲线中止时间。该数值是一个整形，含义为从1970年1月1日
	//上午8点到错误发生时间所经过的秒数。
	long lEndTime;

	//历史曲线显示参数的表号
	unsigned short usParamTable;

	//历史曲线显示参数的参数号
	unsigned short usParamCode;

	//参数传输类型
	unsigned char ucTransType;

	//参数的转换系数
	double dParamQuotiety;

	//参数的表现类型
	unsigned char ucShowType;

};

//指显网络通信设置信息结构体
struct stru_NetConfig 
{
public:
	//指显网组地址
	char sZXGroupIP[16];

	//综合应用分系统通报信息组地址
	char sZH_TBGroupIP[16];

	//东风中心指显组地址
	char sDFZXGroupIP[16];

	//C3I内部参数信息传输协议通信端口号
	unsigned short usZXGroupPort;

	//数据支持网组地址
	char sDSGroupIP[16];

	//数据支持端口号
	unsigned short usDSGroupPort;

	//东风中心辅助支持
	char usASGroupIP[16];

	//数据支持端口号
	unsigned short usASGroupPort;

	//东风中心判读结果
	char usFSGroupIP[16];

	//东风中心判读结果
	unsigned short usFSGroupPort;

	//综合应用数据支持公用组地址
	char sCAGroupIP[16];

	//综合应用数据支持公用端口号
	unsigned short usCAGroupPort;

	//WEB信息查询地址
	char sWebAddress[128];

	//任务代号第一字节
	unsigned char byTask1;

	//任务代号第二字节
    unsigned char byTask2;

	//点火时刻表号
	unsigned short usDHTimeTableNum;

	//点火时刻序号
	unsigned short usDHTimeTableNo;

	//起飞时刻表号
	unsigned short usQFTimeTableNum;

	//起飞时刻序号
	unsigned short usQFTimeTableNo;

	//GPS闰秒
	double dGPSLeapSecond;
};
//指定源组播信息结构体
struct str_IPNetConfig
{
	//组名称
	char sGroupName[64];
	
	//组IP地址
	char sGroupIp[16];
	
	//存储指定的源IP地址
	QStringList SourceIpArray;
};

//画面名称结构体，每个对象对应一个画面名称，画面配置文件对象包
//含多个画面名称结构体。
struct stru_Image 
{
public:
	//画面背景色
	//QColor clBKColor;

	//每幅画面关联一个该画面的显示元素链表。
	CElementList* m_pElementList;

	//画面类型，分为指显工作站浏览画面、综合画面、LED画面、二
	//维数字地图画面、三维视景画面，分别由0、1、2、3、4表示。

	//画面类型，包括：
	//0  专家使用模式；
	//1  测发指挥员使用模式；
	//2  LED操作员使用模式；
	//3  公共投影显示模式；
	//4  其它指挥员显示模式；
	//5  关键技术显示模式;
	//6  二、三维视景画面.
	unsigned char ucImageType;

	//画面X方向上的象素数
	unsigned short usXPix;

	//画面y方向上的象素数
	unsigned short usYPix;

	//综合投影X位置
	unsigned short usXProjPose;

	//综合投影y位置
	unsigned short usYProjPose;

	//画面名称，最大长度64个字节，第65个字符为结束符。
	char sImageName[65];

	//画面代号
	unsigned short usImageCode;

	//led横向像素数（打折处）
	unsigned short usLED;

	//////////////////////////////////////
	//1.11版本增加项
	//2010年03月22日
	//led纵向间隔像素
	unsigned short usLEDZX;

	//画面快捷键
	//Ctrl键
	bool bCtrl;
	//Alt键
//	BOOL bAlt;
	//Shift键
	bool bShift;
	//键的ASCII值
	char Key;

	//背景图
	int usBKPic;		
	
	stru_Image()/*:clBKColor(255, 255, 255)*/
	{
		m_pElementList = NULL;
		ucImageType = 0;
		usXPix= 1280;
		usYPix= 1024;
		usXProjPose= 0;
		usYProjPose= 0;
		sImageName[0]='\0';
		usImageCode=-1;
		usLED=0;
		usLEDZX = 0;
		bCtrl = false;
		bShift = false;
		Key = 0;
		usBKPic = -1;
		memset(sImageName,0,sizeof(sImageName));
	}
};

//参数驱动画面自动显示配置结构体，参数驱动画面自动切换配置文件
//包含多个参数驱动画面自动显示配置结构体。
struct stru_ParamDriveImage 
{
public:
	//参数的表号
	unsigned short usParamTable;

	//参数的序号
	unsigned short usParamCode;

	//画面名
	char* pImageName;

	//关联类型，包括：
	//0   参数值首次到显示画面
	//1   参数值与预设值一致显示画面,在上下限之间
	unsigned char ucType;

	//上限
	double dUpLmt;

	//下限
	double dDownLmt;

	//有效或无效标志
	bool bSUS;
/*	//当关联类型为1（参数值与预设值一致显示画面）的预设值。
	char cParamValue[8];*/
};

//技术支持人员口令存储结构体。
struct stru_Psw 
{
public:
	//技术支持人员口令，最大长度16个字符，第17个字符为结束苻。
	char sPsw[17];
};

//任务阶段信息结构体
struct stru_TaskPhase 
{
public:
	//任务阶段名称
	char* psTaksPhaseName;

	//当前任务标志
	bool bCurrTaskFlag;

};

//任务系列与任务代号关联结构体
struct stru_SeriesTask 
{
public:
	//任务代号字符串指针
	char* psTaskCode;

	//任务ID
	unsigned short usTaskId;

	//任务名称
	char* psTaskName;

	//任务阶段列表
	CElementList* pTaskPhaseList;

};

//状态关联结构，即状态值或参数值区域所代表的填充色，或者一幅图
//片（包括多帧图片，以实现动画效果）。状态关联结构表示的是参数
//与状态表现的一种关联关系。
//    
//状态关联结构通过列表管理类关联于状态指示器显示元素类CStatede
//noteElementCom，即一个状态显示元素可表现一个状态参数的多个状
//态。
class stru_State 
{
public:
	//状态量
	long lState;

	//状态关联图片
	char* StatePicture;
};

struct stru_ZXList 
{
public:
	//链表头指针
	stru_ZXList* ulHead;

	//链表数据
	unsigned long ulData;

	//链表尾指针
	stru_ZXList* ulTail;
};

//时间驱动画面显示配置信息结构体
struct stru_TimeDriveImage 
{
public:
	//有效或无效标志
	bool bSUS;
	//画面名
	char* pImageName;

	//驱动的时间点，单位和精度都是秒，正表示起飞后，负表示起前。//陶小勇 20131017 发测站配置画面中使用
	
	long lDriveTime;
	stru_TimeDriveImage()
	{
		bSUS = true;
		pImageName = NULL;
		lDriveTime = 0;
	}
};

//指显工作站信息结构体，指显工作站地址、工作模式列表文件包含多
//个该结构体。
struct stru_ZXWSFile 
{
public:
	//指显工作站名称，最大长度63个字节，最后一个字节为结束符
	char sZXWSName[64];

	//指显工作站Mac地址，长为17字节。
	char sZXWSMac[18];

	//指显工作站IP地址，长度为15个字节
	char sZXWSIP[16];

	//校时标志
	bool bTimer;

	//该工作站指显软件是否记盘
	bool bRcord;

	//该工作站的本地记盘路径的地址，最大长度512个字节
	char sRcordPath[512];

	//该工作站的最大记盘文件大小,以字节为单位
	unsigned long ulMaxRecordFileLen;

	//该工作站的记盘文件名，最大长度63个字节，最后一个字节为结束符
	char sZXWSRecordFileName[64];

	//工作模式信息，包括：
	//0  专家使用模式；
	//1  测发指挥员使用模式；
	//2  LED操作员使用模式；
	//3  公共投影显示模式；
	//4  其它指挥员显示模式；
	//5  关键技术显示模式。
	//340  显示系统工具栏的模式
	unsigned short ucWorkStateType;

	//显示起始X
	unsigned long ulStartX;

	//显示起始Y
	unsigned long ulStartY;

	//显示横向
	unsigned long ulX;

	//显示纵向
	unsigned long ulY;

	//设备代号
	unsigned long ulWorkCode;

	//连链路监视表号
	unsigned short ulLinkTable;

	//收帧数
	unsigned short ulCode0RcvFrame;

	//收字节数
	unsigned short ulCode1RcvByte;

	//发帧数
	unsigned short ulCode2SndFrame;

	//发字节数
	unsigned short ulCode3SndByte;
};

//更新指显软件配置存储结构体
struct stru_UpdateConfig 
{
public:
	//需要进行更新的指显工作站的IP地址
	char sUpdateIPAdress[16];

	//指显软件的版本。
	char sEdition[64];

	//更新指显软件的FTP地址
	char sUpdateFTPAddress[128];

	//指向C3I网络配置信息的指针
	stru_C3INetInfo* pC3IINetInfo;

	//指向参数驱动画面配置的指针
	stru_ParamDriveImage* pParamDriveImage;

	//指向指显工作站信息结构体的指针
	stru_ZXWSFile* pZXWSFile;
};

//发射系参数结构
struct STRUCT_FA_SHE_XI
{
		double a;	//半长轴，单位是米
		double e2;	//第二偏心率
		double L0;	//发射点经度，单位是度
		double B0;	//发射点纬度，单位是度
		double H0;	//发射点高度，单位是米
		double Aox;	//射向，单位是度
};

//测量系中测控设备信息
struct STRUCT_CLX_POINT
{
	double t;//飞行时间，单位秒
	double l;//经度
	double b;//纬度
	double h;//高程
};

//笛卡尔坐标系弹道数据结构
struct STRUCT_BLTCURVE
{
	double t;	//飞行时间，单位秒
	double x;	//X方向上距离，单位米
	double y;	//Y方向上距离，单位米
	double z;	//Z方向上距离，单位米
	double vx;	//X方向上速度，单位米/秒
	double vy;	//X方向上速度，单位米/秒
	double vz;	//X方向上速度，单位米/秒
};

//REA
struct STRUCT_REA
{
	double t;//飞行时间，单位秒
	double r;//测距，单位米
	double e;//俯仰角，单位度
	double a;//方位角，单位度
};

//图元工具条项结构体
struct 	stru_ImageGraphicGroup 
{
public:		
	//图元工具条项代号（1到63）
	unsigned char ucID;
	//图元工具条项图标地址（21个字符，其中第21个字符是结束符）
	char sPath[64];
	//图元工具条项名称（9个字符，其中第九个字符是结束符）
	char sName[64];
};


//测发流程显示节点结构
struct stru_CMDFlow
{
	WORD	wTable;				//表号
	WORD	wParam;				//参数号
	DWORD	dwValue;			//参数值
	char*	pShowTxt;			//显示文字
	char*	pNoteTxt;			//说明文字
	void*	pTestParamList;		//测试结果参数列表，每个节点存放参数地址
	//节点状态，0表示没到值，1表示当前值，2表示非当前值，
	//3表示当前值但是已经不闪烁了
	unsigned char	byState;			
	DWORD	dwShowTestParamPos;	//显示测试结果参数最后一个参数的位置
	int		iListPos;			//在LIST中的位置
	char	sTime[13];			//值到的时间
	bool	bAdd;				//是否已经向临时列表中添加该项
	long	lXDTime;			//相对时间，当这个时间为LONG_MAX 2147483647 表示不受时间限制
								//否则，尽当参数时间减去预计点火时间的值大于等于该值时才有效
	CElementList* pElementList;	//每个测试项目的关联图元
	stru_CMDFlow()
	{
		wTable = 0;
		wParam = 0;
		dwValue = 0;
		pShowTxt = NULL;
		pNoteTxt = NULL;
		pTestParamList = NULL;
		byState = 0;
		dwShowTestParamPos = 0;
		iListPos = -1;
		sTime[0] = 0;
		bAdd = false;
		lXDTime = LONG_MAX;

		pElementList = NULL;
	}
// 	~stru_CMDFlow()
// 	{
// 		delete pShowTxt;pShowTxt=NULL;
// 		delete pNoteTxt;pNoteTxt=NULL;
// 	}
};
//存放表号，参数号的结构
struct stru_TestResult
{
	WORD tabno;
	WORD paramno;
	char*	paramname;	
	DWORD*	value;			
	char*	resulttext;	
    AbstractParam* pParam;
	stru_TestResult()
	{
		tabno = 0;
		paramno = 0;		
		paramname = NULL;
		value = NULL;
		resulttext = NULL;
		pParam = NULL;
	}
};

//
struct stru_ParamTemp 
{
	char ucIndex[7];
	
	stru_ParamTemp()
	{
		for (int i=0;i<7;i++)
		{
			ucIndex[i] = -1;
		}
	}
};

//
struct stru_ParamtoParamTemp 
{
	CElementList* pParamList;
	CElementList* pParamtempList;

	stru_ParamtoParamTemp()
	{
		pParamList = NULL;
		pParamtempList = NULL;
	}
};
//T0 时间 结构体
struct T0Struct 
{
	QString name;//T0名称
	WORD dh_bh;//点火表号
	WORD dh_xh;//点火序号
	WORD qf_bh;//起飞表号
	WORD qf_xh;//起飞序号
	DWORD qf_invalid;//起飞时刻无效值
	WORD t0_ms_bh;//T0模式表号
	WORD t0_ms_xh;//T0模式序号
	std::vector<DWORD > t0_ms_invalid_vc;//T0模式无效值列表
    AbstractParam * dh_param;//点火参数
    AbstractParam * qf_param;//起飞参数
	//为了解决东风中心发现的BUG：
	//T0时间为0，T0模式为0的情况下，
	//在某些指显终端上偶尔出现，
	//T0时间闪烁显示“00时00分00秒000毫秒”的现象
	//新增T0时间临时存放参数的位置
	//刘裕贵、陶小勇，2013年10月22日
    AbstractParam * qf_param_back;//起飞参数的临时存放位置
    AbstractParam * t0_ms_param;//T0模式参数
	std::vector<void *> gl_ty;//关联图元
	bool t0_b;//T0是否有效
	DWORD t0_old_value;//起飞参数旧值
	DWORD t0_qf_date;//起飞参数日期
	DWORD t0_qf_time;//起飞参数时间
	DWORD dh_old_value;//点火参数旧值
	DWORD t0_dh_date;//点火参数日期
	DWORD t0_dh_time;//点火参数时间
	T0Struct()
	{
		name = "";
		dh_bh = 0;
		dh_xh = 0;
		qf_bh = 0;
		qf_xh = 0;
		qf_invalid = 0;
		t0_ms_bh = 0;
		t0_ms_xh = 0;
		dh_param=NULL;
		qf_param=NULL;	
		qf_param_back=NULL;
		t0_ms_param=NULL;
		t0_b = false;
		//需要初始值无效
		t0_old_value = 0xffffffff;
		dh_old_value = 0xffffffff;
		t0_qf_date = 0xffffffff;
		t0_qf_time = 0xffffffff;
		t0_dh_date = 0xffffffff;
		t0_dh_time = 0xffffffff;
	};
};
//BCD 码
struct BCDTime
{
	unsigned MS       :4;//毫秒的个位
	unsigned MSD      :4;//毫秒的十位
	unsigned MSP      :4;//毫秒的百位
	unsigned S        :4;//秒的个位
	unsigned SD       :3;//秒的十位
	unsigned M        :4;//分的个位
	unsigned MD       :3;//分的十位
	unsigned H        :4;//时的个位
	unsigned HD       :2;//时的十位
};
//PMTime码为0.1毫秒的整数
#define PMTime unsigned long
//UTCTime码为单位为秒的double
#define UTCTime unsigned long
// struct stru_Param_ParamTemp
// {
// 	stru_Param* pParam;
// 	stru_ParamTemp* pParamTemp;
// 	stru_Param_ParamTemp()
// 	{
// 		pParam = NULL;
// 		pParamTemp = NULL;
// 	}
// };


