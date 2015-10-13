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
HistoryParamBuffer::HistoryParamBuffer():m_binSem(1)
{
	int res = 0;

    m_listLimit = 50000;

	//初始化缓冲区队列
	m_HistoryParamBuf.clear();

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
HistoryParamBuffer::~HistoryParamBuffer()
{
	//清空缓冲区队列
	m_HistoryParamBuf.clear();

	//sem_destroy(&m_binSem);
}

//******************
//说明：将参数加入缓冲区
//功能：将一个参数加入缓冲区队列
//备注：前面进，后面出
//******************
void HistoryParamBuffer::PutBuffer(unsigned short tn,unsigned short pn,HistoryParam& buf)
{
	//sem_wait(&m_binSem);
    m_binSem.acquire();
    m_HistoryParamBuf[tn][pn].push_front(buf);
    int temp = m_savListLimit[tn][pn];
    if(temp >= m_listLimit)
    {
        m_HistoryParamBuf[tn][pn].pop_back();
    }
    else
    {
        m_savListLimit[tn][pn]++;
    }

	//sem_post(&m_binSem);
    m_binSem.release();
}

//******************
//说明：从缓冲区取出参数
//功能：从缓冲区队列取出参数
//备注：前面进，后面出
//******************
list<HistoryParam> HistoryParamBuffer::GetBuffer(unsigned short tableno, unsigned short paramno)
{
	list<HistoryParam> buf;

	//sem_wait(&m_binSem);
    m_binSem.acquire();
	
    buf = m_HistoryParamBuf[tableno][paramno];

	//sem_post(&m_binSem);
    m_binSem.release();

	return buf;
}

list<HistoryParam> HistoryParamBuffer::GetBuffer(unsigned short tableno, unsigned short paramno,int & date,int & time)
{
    //sem_wait(&m_binSem);
    m_binSem.acquire();
    list<HistoryParam> buf;
    list<HistoryParam> retBuf;
    list<HistoryParam>::reverse_iterator it;
    buf = m_HistoryParamBuf[tableno][paramno];
    //qDebug()<<"Date " << date << "Time " << time ;
    int t_date,t_time=0;
    //倒序取数据
    for (it = buf.rbegin();it != buf.rend();it++)
    {
        if (((*it).getDate() > date)||(((*it).getDate() == date)&&((*it).getTime() > time)))
        {
            retBuf.push_back(*it);
            if(t_time<(*it).getTime())
            {
                t_date = (*it).getDate();
                t_time = (*it).getTime();
            }
            //qDebug()<<(*it).getValue() ;
        }
    }
    if(t_time!=0)
    {
        date = t_date;
        time = t_time;
    }
    //qDebug()<<"Size " << retBuf.size();
    //sem_post(&m_binSem);
    m_binSem.release();
    return retBuf;
}
