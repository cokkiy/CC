// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：netbuffer.cpp
// 文件摘要：存储网络数据缓冲区类实现文件，用来实现存储网络数据缓冲区类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#include "netbuffer.h"
#include <errno.h>

extern int errno;
//******************
//说明：缓冲区类构造函数
//功能：初始化各成员变量
//备注：
//******************
NetBuffer::NetBuffer(): m_binSem(1) 
{
	int res = 0;

	//初始化缓冲区队列
	m_netBuf.clear();

	//默认缓存5000000帧, 大约500M
	m_bufLimit = 5000000;

	m_listNum = 0;

    
	//初始化缓冲区操作同步信号量
	//res = sem_init(&m_binSem, 0, 1);
	/*if(res != 0)
	{
		m_errorCode = errno;
	}*/
}

//******************
//说明：缓冲区类析构函数
//功能：释放给缓冲区队列所占用空间
//备注：无
//******************
NetBuffer::~NetBuffer()
{
	//清空缓冲区队列
	m_netBuf.clear();

	m_listNum = 0;

	//sem_destroy(&m_binSem);
}

//******************
//说明：设置缓冲区队列参数
//功能：设置缓冲区队列长度限制
//备注：无
//******************
void NetBuffer::SetBufListLimit(int limit)
{
	m_bufLimit = limit;
}

//******************
//说明：加入缓冲区
//功能：将一个缓冲区加入缓冲区队列
//备注：前面进，后面出
//******************
void NetBuffer::PutBuffer(Buffer& buf)
{
	//sem_wait(&m_binSem);
    m_binSem.acquire();

	m_netBuf.push_front(buf);
	m_listNum++;
	if(m_listNum > m_bufLimit) 
	{
		m_netBuf.pop_back();
		m_listNum--;
	}

	//sem_post(&m_binSem);
    m_binSem.release();
}

//******************
//说明：取出缓冲区
//功能：从缓冲区队列取出缓冲区
//备注：前面进，后面出
//******************
Buffer NetBuffer::GetBuffer()
{
	Buffer buf;

	//sem_wait(&m_binSem);
    m_binSem.acquire();
	if(false == m_netBuf.empty())
	{
		buf = m_netBuf.back();
		m_netBuf.pop_back();
		m_listNum--;
	}
	//sem_post(&m_binSem);
    m_binSem.release();

	return buf;
}


