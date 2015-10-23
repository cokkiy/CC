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


/*!
获取date.time之后的数据?
@param unsigned short tn 表号
@param unsigned short pn 参数号
@param int & date 日期
@param int & time 时间
@return list<HistoryParam>按时间从前向后排序的参数列表
作者：cokkiy（张立民)
修改时间：2015/10/23 10:25:55
*/
list<HistoryParam> HistoryParamBuffer::GetBuffer(unsigned short tn, unsigned short pn, int & date, int & time)
{
    list<HistoryParam> retBuf;
    HistoryParams::reverse_iterator it;
    auto& param = m_HistoryParamBuf[INDEX(tn,pn)];
    HistoryParams* pbuf = param.getBuffer();
    int t_date,t_time=0;
    //从最后一个vector开始取数据
    for (it = pbuf->rbegin(); it != pbuf->rend(); it++)
    {
        vector<HistoryParam>* pVector = *it;
        HistoryParam& param = pVector->front();
        size_t count = pVector->size();
        if (param.getDate() > date || (param.getDate() == date&&param.getTime() > time))
        {
            //复制该vector中全部数据到list,retBuf是按时间从前到后排序的
            for(auto iter=pVector->begin();iter<pVector->begin()+count;iter++)
            {
                retBuf.push_back(*iter);
            }
            //retBuf.insert(retBuf.begin(), pVector->begin(), pVector->begin() + count-1);
            t_date = param.getDate();
            t_time = param.getTime();
        }
        else
        {
            //找到vector中的某个元素,使其时间小于等于date.time
            for (auto iter = pVector->end() - 1; iter == pVector->begin(); iter--)
            {
                if (iter->getDate() > date || (iter->getDate() == date&&iter->getTime() > time))
                {
                    //把该参数放到retBuf前面,retBuf是按时间从前到后排序的
                    retBuf.push_front(*iter);
                    t_date = iter->getDate();
                    t_time = iter->getTime();
                }
                else
                {
                    break;
                }
            }
        }
    }    
    if(t_time!=0)
    {
        date = t_date;
        time = t_time;
    }
    return retBuf;
}
