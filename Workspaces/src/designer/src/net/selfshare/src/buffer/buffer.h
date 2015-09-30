// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：buffer.h
// 文件摘要：综合应用分系统缓冲区类头文件，用来声明缓冲区类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef BUFFER_H
#define BUFFER_H

#include<list>
#include "../const.h"																									//常数定义头文件
#include "../baseobject.h"																						//基类头文件

using namespace std;

class Buffer :public BaseObject
{
	public:
		Buffer();																													//普通缓冲区类无参数构造函数
		Buffer(const Buffer &a);																					//拷贝构造函数
		Buffer(unsigned short size);																			//普通缓冲区类构造函数
		~Buffer();
		bool WriteBuffer(const char *src, unsigned short size);
		char *ReadBuffer(unsigned short size);
		void ClearBuffer();
		bool InitializeBuffer(unsigned short size);  											//初始化缓冲区，本操作与无参数构造函数配套使用
		char *GetBufAddr();
		bool IsEmpty();
		unsigned short GetBufSize();
		unsigned short GetWritePos();
		
		Buffer & operator =(const Buffer &a);															// 赋值函数
		
	private:
		char *m_buffer;																										//缓冲区指针
		unsigned short m_bufSize;																					//缓冲区大小

		unsigned short m_writePosition;																		//缓冲区内容的大小

};

typedef list<Buffer> BufferList;

#endif


