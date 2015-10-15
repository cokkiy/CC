// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：zxparambuffer.h
// 文件摘要：指显系统参数缓冲区类头文件，用来声明参数缓冲区类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef ZXPARAMBUFFER_H
#define ZXPARAMBUFFER_H

#include <qsemaphore.h>   //  cokkiy 15-09-06  #include <semaphore.h>
#include <list>
#include "../../../abstractparam.h"
#include "../baseobject.h"	
#include <map>
#include <unordered_map>
using namespace std;

//定义ParaMap类型,表示某表号下的所有参数集合(按参数号组织)
typedef std::map<unsigned short, AbstractParam> ParaMap;
//定义ParaMap类型,表示按表号,参数号组织的所有参数集合
typedef std::unordered_map<unsigned short, ParaMap > TableMap;
//
typedef std::unordered_map<unsigned int, AbstractParam*> CacheTableMap;
class ZXParamBuffer :public BaseObject
{
	private:

        //缓冲区Map
        TableMap m_ZXparamBuf;

		//缓冲区读写同步信号量
		QSemaphore m_binSem;

	public:

		//构造函数
		ZXParamBuffer();																													//普通缓冲区类无参数构造函数

		//析构函数
		~ZXParamBuffer();

		//将缓冲区加入队列
        void PutBuffer(AbstractParam& zxParam);

		//从队列中取出缓冲区
        AbstractParam GetBuffer(unsigned short tabno, unsigned short paramno);
        AbstractParam* GetParamBuffer(unsigned short tabno, unsigned short paramno);
        TableMap* GetAllParamMapPoint();
};


#endif
