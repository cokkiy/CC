// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：historyparambuffer.h
// 文件摘要：历史参数缓冲区类头文件，用来声明历史参数缓冲区类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef HISTORYPARAMBUFFER_H
#define HISTORYPARAMBUFFER_H

#include <qsemaphore.h>
#include <list>
#include <mutex>
#include <map>
#include "../historyparam/historyparam.h"

using namespace std;


#define INDEX(tn,pn) (((unsigned int)tn<<16)|(unsigned short)pn)

class HistoryParamBuffer : public BaseObject
{
public:

	//构造函数
	HistoryParamBuffer();																													
	//析构函数
	~HistoryParamBuffer();

	//将缓冲区加入队列
	void PutBuffer(unsigned short tn,unsigned short pn,HistoryParam& hParam);

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
    std::list<HistoryParam> GetBuffer(unsigned short tableno, unsigned short paramno,int & date,int & time);
};


#endif


