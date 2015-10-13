// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：historyparam.h
// 文件摘要：历史参数类头文件，用来声明历史参数类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef HISTORYPARAM_H
#define HISTORYPARAM_H

#include "../baseobject.h"	
#include "../zxparam/zxparam.h"

class HistoryParam : public BaseObject
{
	private:
		//参数日期
        unsigned int m_Date;
		//参数时间
        unsigned int m_Time;
		//参数值
        double m_Val;

	public:
		//构造函数
		HistoryParam();	
		//拷贝构造函数
		HistoryParam(const HistoryParam &a);	
		//析构函数
		~HistoryParam();

        //取出日期
        void setDate(unsigned int date){m_Date = date;}

        //取出时间
        void setTime(unsigned int time){m_Time = time;}

        //取出时间
        void setValue(double val){m_Val = val;}

        //取出日期
        unsigned int getDate(){return m_Date;}

        //取出时间
        unsigned int getTime(){return m_Time;}

        //取出时间
        double getValue(){return m_Val;}

		//赋值函数
		HistoryParam& operator =(const HistoryParam &a);	

};

#endif
