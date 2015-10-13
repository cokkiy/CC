// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：thread.h
// 文件摘要：综合应用分系统线程类头文件，用来声明线程类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef THREAD_H
#define THREAD_H

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <QThread>
#include <QSemaphore>

using namespace std;

#include "../datastruct.h"

class PThread:public QThread
{
	public:
		PThread();												//线程类构造函数
		virtual ~PThread();
        virtual void run();								//线程内容函数
		void OnHold();											//挂起线程
		void Resume();											//恢复线程运行
		
	protected:
        QSemaphore  m_ctrlBinSem;						//线程控制信号量
};



#endif
