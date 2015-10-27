// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：zxparambuffer.cpp
// 文件摘要：存储指显参数缓冲区类实现文件，用来实现存储指显参数缓冲区类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 修改： cokkiy 2015-10-24
// 完成日期：

#include "historyparambuffer.h"
#include "HistoryBufferManager.h"

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
}

//******************
//说明：将参数加入缓冲区
//功能：将一个参数加入缓冲区队列
//备注：前面进，后面出
//******************
void HistoryParamBuffer::PutBuffer(unsigned short tn, unsigned short pn, HistoryParam& buf)
{
    HistoryBufferManager::PushParam(tn, pn, buf);
}

//******************
//说明：从缓冲区取出参数
//功能：从缓冲区队列取出参数
//备注：前面进，后面出
//******************
// HistoryParams HistoryParamBuffer::GetBuffer(unsigned short tn, unsigned short pn)
// {
// 	HistoryParams buf;
//     auto& param = m_HistoryParamBuf[INDEX(tn,pn)];
//     buf = *(param.getBuffer());
//     return buf;
// }


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
    return HistoryBufferManager::getParams(tn, pn, date, time);
}
