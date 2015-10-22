// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：zxparambuffer.cpp
// 文件摘要：存储指显参数缓冲区类实现文件，用来实现存储指显参数缓冲区类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#include "historyparambuffer.h"
#include <errno.h>
#include <QDebug>
extern int errno;
//******************
//说明：缓冲区类构造函数
//功能：初始化各成员变量
//备注：
//******************
HistoryParamBuffer::HistoryParamBuffer()
{
}

//******************
//说明：缓冲区类析构函数
//功能：释放给缓冲区队列所占用空间
//备注：无
//******************
HistoryParamBuffer::~HistoryParamBuffer()
{
    //释放历史缓冲区空间
    HistoryParamMap::iterator it = m_HistoryParamBuf.begin();
    for(;it!=m_HistoryParamBuf.end();it++)
    {
        GCWrapper& wrapper = it->second;
        wrapper.release();
    }
	//清空缓冲区队列
	m_HistoryParamBuf.clear();
}

//******************
//说明：将参数加入缓冲区
//功能：将一个参数加入缓冲区队列
//备注：前面进，后面出
//******************
void HistoryParamBuffer::PutBuffer(unsigned short tn,unsigned short pn,HistoryParam& buf)
{    
    auto& param = m_HistoryParamBuf[INDEX(tn,pn)];   
    lockForWriteBuf.lock();
    param.push(buf);
    lockForWriteBuf.unlock();
}

//******************
//说明：从缓冲区取出参数
//功能：从缓冲区队列取出参数
//备注：前面进，后面出
//******************
HistoryParams HistoryParamBuffer::GetBuffer(unsigned short tn, unsigned short pn)
{
	HistoryParams buf;
    auto& param = m_HistoryParamBuf[INDEX(tn,pn)];
    buf = *(param.getBuffer());
    return buf;
}

HistoryParams HistoryParamBuffer::GetBuffer(unsigned short tn, unsigned short pn,int & date,int & time)
{
    HistoryParams retBuf;
    HistoryParams::reverse_iterator it;
    auto& param = m_HistoryParamBuf[INDEX(tn,pn)];
    HistoryParams buf = *(param.getBuffer());
    int t_date,t_time=0;
    //倒序取数据
    for (it = buf.rbegin();it != buf.rend();it++)
    {
//         if (((*it).getDate() > date)||(((*it).getDate() == date)&&((*it).getTime() > time)))
//         {
//             retBuf.push_back(*it);
//             if(t_time<(*it).getTime())
//             {
//                 t_date = (*it).getDate();
//                 t_time = (*it).getTime();
//             }
//         }
    }
    if(t_time!=0)
    {
        date = t_date;
        time = t_time;
    }
    return retBuf;
}
