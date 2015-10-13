﻿// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：thread.cpp
// 文件摘要：综合应用分系统线程类实现文件，用来实现线程类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#include "../const.h"
#include "thread.h"

//*********************************
//线程类构造函数
//
//
//*********************************
PThread::PThread() :QThread(), m_ctrlBinSem(1)
{
}
	

//*********************************
//线程类析构函数
//
//
//*********************************
PThread::~PThread()
{
}

//*********************************
//线程内容函数
//
//
//*********************************
void PThread::run()
{

}

//*********************************
//挂起线程
//
//
//*********************************
void PThread::OnHold()								
{
    m_ctrlBinSem.acquire();
}

//*********************************
//恢复线程运行
//
//
//*********************************		
void PThread::Resume()										
{
    m_ctrlBinSem.release();
}







