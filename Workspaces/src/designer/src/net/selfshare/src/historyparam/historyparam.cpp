// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：historyparam.cpp
// 文件摘要：历史参数类头文件，用来实现历史参数类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#include <string.h>
#include <stdlib.h>

#include "historyparam.h"

//******************
//说明：历史参数类构造函数
//功能：初始化各成员变量
//备注：
//******************
HistoryParam::HistoryParam():BaseObject()
{
    m_Date = 0;
    m_Time = 0;
    m_Val = 0.0;
	m_errorCode = 0;
}

//******************
//说明：拷贝构造函数
//功能：通过拷贝初始化一个历史参数类实例
//备注：主要是在模版类（如list,vector等）中push操作中使用
//******************
HistoryParam::HistoryParam(const HistoryParam &a):BaseObject()
{
    m_Date = a.m_Date;
    m_Time = a.m_Time;
    m_Val = a.m_Val;
    m_errorCode = 0;								//复位错误代码
}

//******************
//说明：析构函数
//功能：
//备注：
//******************
HistoryParam::~HistoryParam()
{

}

//******************
//说明：历史参数类的赋值构造函数
//功能：通过赋值操作初始化一个历史参数实例
//备注：无
//******************
HistoryParam & HistoryParam::operator =(const HistoryParam &a)
{
	if(this == &a) return *this;									//如果是自赋值，返回自身
    m_Date = a.m_Date;
    m_Time = a.m_Time;
    m_Val = a.m_Val;
	m_errorCode = 0;															//复位错误代码
	return *this;
}
