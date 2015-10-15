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
#include <map>
#include <vector>
#include <mutex>
#include <unordered_map>
#include "../historyparam/historyparam.h"
#include "../baseobject.h"	

using namespace std;

typedef list<HistoryParam> HistoryParams;
struct HistoryParamList
{
    HistoryParams* pParamsBuf;
    HistoryParamList()
    {
        pParamsBuf = new HistoryParams();
    }

    HistoryParamList(const HistoryParamList& ref)
    {
        this->pParamsBuf = ref.pParamsBuf;
    }
//     ~HistoryParamList()
//     {
//         if (pParamsBuf != NULL)
//             delete pParamsBuf;
//     }
};
typedef map<unsigned int, HistoryParamList > HistoryParamMap;
typedef map<unsigned int,  int > HistoryParamLimit;

class HistoryParamBuffer : public BaseObject
{
private:

	//缓冲区队列
	HistoryParamMap m_HistoryParamBuf;	

	//缓冲区读写同步信号量 cokkiy 09-07 修改
	//QSemaphore m_binSem;
    mutex lockForWriteBuf;

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
    HistoryParams GetBuffer(unsigned short tableno, unsigned short paramno);
    //从队列中取出缓冲区
    HistoryParams GetBuffer(unsigned short tableno, unsigned short paramno,int & date,int & time);
    map<unsigned short, map<unsigned short, mutex>> locks;
};




#endif


