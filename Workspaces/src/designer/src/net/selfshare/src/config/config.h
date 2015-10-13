// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：config.h
// 文件摘要：配置信息类头文件，用来声明配置信息类。
// 
// 原始版本：1.0
// 当前版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef CONFIG_H
#define CONFIG_H

#include "../buffer/buffer.h"
#include "../xmlproc/xmlproc.h"
#include "../const.h"
#include "../datastruct.h"
#include "../baseobject.h"
//#include "../../../main/src/frame/frame.h"
#include "../netbuffer/netbuffer.h"
#include "../historyparambuffer/historyparambuffer.h"
#include "../zxparambuffer/zxparambuffer.h"
#include "../../../net_global.h"

using namespace std;

class NETSHARED_EXPORT Config:public BaseObject
{
public:
	Config();
	~Config();

	//加载配置文件
    bool LoadConfig(QString dir);
    bool LoadTableInfo(const char* path);


public:
    int  m_threadNum;  							//设备依据线程的分组
    //InfoTableProc 	m_infoTabProc;															//信息约定表处理类 主要负责加载信息约定表并提供信息约定表的指针
    //int m_absoluteTime, m_taskDate;                        //绝对时和任务日期

//	unsigned char m_infoSrc/*信源*/,
//			      m_taskCode/*任务代号*/,
//				  m_hlCode/*合练代号*/,
//				  m_infotype/*信息类别*/;

//	unsigned short m_recvMaxFrameLen;										//接收最大帧长
//	unsigned short m_sendMaxFrameLen;										//发送最大帧长

//	//网络设置
//	unsigned int m_ipAddr;			//服务器套接字地址
//	unsigned short m_port;		//接收端口号
//	list<unsigned int> m_groupAddr;	//需加入的组地址

								

	//指显参数缓冲区队列
	ZXParamBuffer m_zxParamBuf;

	//历史参数缓冲区队列
	HistoryParamBuffer m_zxHistoryParamBuf;

    map<unsigned short, string> m_tableInfo;
};

#endif


