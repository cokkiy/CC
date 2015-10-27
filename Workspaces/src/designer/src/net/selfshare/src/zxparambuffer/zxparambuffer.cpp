// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：zxparambuffer.cpp
// 文件摘要：存储指显参数缓冲区类实现文件，用来实现存储指显参数缓冲区类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#include "zxparambuffer.h"
#include <errno.h>

extern int errno;
//******************
//说明：缓冲区类构造函数
//功能：初始化各成员变量
//备注：
//******************
ZXParamBuffer::ZXParamBuffer(): m_binSem(1)
{
	//初始化缓冲区队列
	m_ZXparamBuf.clear();
}

//******************
//说明：缓冲区类析构函数
//功能：释放给缓冲区队列所占用空间
//备注：无
//******************
ZXParamBuffer::~ZXParamBuffer()
{
}

//******************
//说明：将参数加入缓冲区
//功能：将一个参数加入缓冲区队列
//备注：前面进，后面出
//******************
void ZXParamBuffer::PutBuffer(AbstractParam& buf)
{
	//sem_wait(&m_binSem);
    m_binSem.acquire();
    m_ZXparamBuf[buf.GetTableNo()][buf.GetParamNo()] = buf;

	//sem_post(&m_binSem);
    m_binSem.release();
}

//******************
//说明：从缓冲区取出参数
//功能：从缓冲区队列取出参数
//备注：前面进，后面出
//******************
AbstractParam ZXParamBuffer::GetBuffer(unsigned short tabno, unsigned short paramno)
{
    AbstractParam buf;

	//sem_wait(&m_binSem);
    m_binSem.acquire();
	
    buf = m_ZXparamBuf[tabno][paramno];

	//sem_post(&m_binSem);
    m_binSem.release();

	return buf;
}

//******************
//说明：从缓冲区取出参数
//功能：从缓冲区队列取出参数
//备注：前面进，后面出
//******************
AbstractParam* ZXParamBuffer::GetParamBuffer(unsigned short tabno, unsigned short paramno)
{
    return &(m_ZXparamBuf[tabno][paramno]);
}

TableMap* ZXParamBuffer::GetAllParamMapPoint()
{
    return &m_ZXparamBuf;
}

