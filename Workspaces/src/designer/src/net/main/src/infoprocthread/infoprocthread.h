// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：infoprocthread.h
// 文件摘要：综合应用分系统信息处理进程信息处理线程类头文件，用来声明信息处理线程类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef INFOPROCINFOPROCTHREAD_H
#define INFOPROCINFOPROCTHREAD_H

#include "../../../selfshare/src/thread/thread.h"
#include "net_global.h"
#include "../../../selfshare/src/config/netconfig.h"
class NETSHARED_EXPORT InfoProcThread :public PThread
{
public:
    InfoProcThread(); 																			//构造函数
    void SetSerialNo(int no);
    virtual void run();																			//线程内容函数
    bool ReportError(const unsigned short Errorcode);  //报错函数
    //设置网络配置文件
    void SetNetConfig(NetConfig*);

    //该线程是否处理主用通道数据,<c.>true</c>表示处理的主用通道的数据,
    // <c>false</c>false表示处理的备用通道的数据
    bool isParimaryDataThread;

private:
    InfoProcThread(const InfoProcThread &a);								// 私有的拷贝构造函数

private:
    int m_serialNo;

    //InputFrame 	inputFrame;									  //输入帧				

    //Parameter param;

    //char HintInfo[200];

    //网络配置文件
    NetConfig* m_NetConfig;
};


#endif
