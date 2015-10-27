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

//#include "../buffer/buffer.h"
//#include "../xmlproc/xmlproc.h"
#include "../const.h"
//#include "../datastruct.h"
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
	//指显参数缓冲区队列
	ZXParamBuffer m_zxParamBuf;

	//历史参数缓冲区队列
	HistoryParamBuffer m_zxHistoryParamBuf;

    map<unsigned short, string> m_tableInfo;
};

#endif


