// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：recvinfothread.h
// 文件摘要：综合应用分系统网络收发进程接收信息线程类头文件，用来声明接收信息线程类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef RECVINFOTHREAD_H
#define RECVINFOTHREAD_H

#include "../../../selfshare/src/thread/thread.h"
#include "net_global.h"
using namespace std;

class NETSHARED_EXPORT RecvInfoThread:public PThread
{
	public:
		RecvInfoThread(); 								//构造函数

        //为信息接收线程设置工作用的Network对象
        void setNetwork(Network*);
		
        //线程内容函数
        void run();								

		
	private:
		RecvInfoThread(const RecvInfoThread &a);				// 私有的拷贝构造函数

        //主用通道处理线程序号
		unsigned char m_primaryThreadNo;
        //备用通道处理线程序号
        unsigned char m_backupThreadNo;

        //网络接口
        Network *ni;

        //检查是否是有效的数据包
        bool isValidPacket(int dataLen,char* buf);
};



#endif
