// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：netbuffer.h
// 文件摘要：存储网络数据缓冲区类头文件，用来声明存储网络数据缓冲区类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef NETBUFFER_H
#define NETBUFFER_H

#include <QSemaphore>									//线程同步信号量头文件 ,cokkiy 用#include <QSemaphore>代替 semaphore
#include<list>
#include "../const.h"										//常数定义头文件
#include "../baseobject.h"									//基类头文件
#include "../buffer/buffer.h"								//基础缓冲区类

using namespace std;

class NetBuffer :public BaseObject
{
	private:
		
		//缓冲区队列
		BufferList m_netBuf;	

		//缓冲区队列长度
		int m_listNum;

		//缓冲区长度限制
		int m_bufLimit;

		//缓冲区读写同步信号量
		QSemaphore m_binSem;

	public:

		//构造函数
		NetBuffer();																													//普通缓冲区类无参数构造函数
		
		//析构函数
		~NetBuffer();

		//设置缓冲区队列长度限制
		void SetBufListLimit(int limit);

		//将缓冲区加入队列
		void PutBuffer(Buffer& buf);

		//从队列中取出缓冲区
		Buffer GetBuffer();

};

#endif

