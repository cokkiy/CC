// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：historyparambuffer.h
// 文件摘要：历史参数缓冲区类头文件，用来声明历史参数缓冲区类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef HISTORYPARAMBUFFER_H
#define HISTORYPARAMBUFFER_H

//#include <semaphore.h>
#include <qsemaphore.h>
#include <list>
#include "../historyparam/historyparam.h"
#include "../baseobject.h"	

using namespace std;

typedef map<unsigned short, map<unsigned short,  list<HistoryParam> > > HistoryParamMap;
typedef map<unsigned short, map<unsigned short,  int > > HistoryParamLimit;

class HistoryParamBuffer : public BaseObject
{
private:

	//缓冲区队列
	HistoryParamMap m_HistoryParamBuf;	

	//缓冲区读写同步信号量 cokkiy 09-07 修改
	QSemaphore m_binSem;

    //
    int m_listLimit;

    HistoryParamLimit m_savListLimit;

public:

	//构造函数
	HistoryParamBuffer();																													//普通缓冲区类无参数构造函数

	//析构函数
	~HistoryParamBuffer();

	//将缓冲区加入队列
	void PutBuffer(unsigned short tn,unsigned short pn,HistoryParam& hParam);

	//从队列中取出缓冲区
	list<HistoryParam> GetBuffer(unsigned short tableno, unsigned short paramno);
    //从队列中取出缓冲区
    list<HistoryParam> GetBuffer(unsigned short tableno, unsigned short paramno,int & date,int & time);
};


#endif


