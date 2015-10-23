// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：buffer.cpp
// 文件摘要：综合应用分系统缓冲区类实现文件，用来实现缓冲区类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#include <string.h>
#include <stdlib.h>

#include "buffer.h"

//******************
//说明：缓冲区类构造函数
//功能：初始化各成员变量
//备注：缓冲区指针变量不分配空间
//******************
Buffer::Buffer():BaseObject()
{
	m_buffer = NULL; 
	m_writePosition = 0;		   									//存储缓冲区当前的写位置
	m_bufSize = 0;
	m_errorCode = 0;														//复位错误代码
}

//******************
//说明：初始化缓冲区
//功能：按给定的参数size的大小初始化缓冲区
//备注：无
//******************
bool Buffer::InitializeBuffer(unsigned short size)
{
	if(NULL != m_buffer) delete []m_buffer;				//释放旧指针
	m_buffer = new char[size];										//分配空间
	
	if(NULL == m_buffer) 													//内存不足
	{
		m_errorCode = USR_NOT_ENOUGH_MEMORY;				//存储错误代码
		return false;
	}
	else
	{
		m_writePosition = 0;												//数据区内容长度清零
		m_bufSize = size;	   												//存储缓冲区大小
		m_errorCode = 0;														//复位错误代码
		return true;
	}
}

//******************
//说明：拷贝构造函数
//功能：通过拷贝初始化一个缓冲区实例
//备注：主要是在模版类（如list,vector等）中push操作中使用
//******************
Buffer::Buffer(const Buffer &a):BaseObject()
{
	if((0 == a.m_bufSize)&&(NULL == a.m_buffer))		//被拷贝对象无内容
	{
		m_buffer = NULL; 						//缓冲区指针置为空指针
		m_writePosition = 0;		   	//存储缓冲区当前的写位置
		m_bufSize = 0;							//缓冲区大小清零
		m_errorCode = 0;	
	}
	else
	{
		m_bufSize = a.m_bufSize;			//拷贝缓冲区大小
		m_buffer = new char[m_bufSize];												//分配空间
	
		if(NULL == m_buffer)  													//内存不足
		{
			m_errorCode = USR_NOT_ENOUGH_MEMORY;					//存储错误代码
			m_bufSize = 0;
		}
		else
		{
			memcpy(m_buffer, a.m_buffer , a.m_bufSize);
			m_writePosition = a.m_writePosition;		   	//存储缓冲区当前的写位置
			m_errorCode = 0;														//复位错误代码
		}
	}
}

//******************
//说明：缓冲区类构造函数
//功能：初始化各成员变量
//备注：缓冲区指针变量分配空间
//******************
Buffer::Buffer(unsigned short size)
{
//缓冲区初始化
	m_buffer = new char[size]; 												//分配空间

	if(NULL == m_buffer)   													//内存不足
	{
		m_errorCode = USR_NOT_ENOUGH_MEMORY;						//存储错误代码
	}
	else
	{
		m_bufSize = size;	   	//存储缓冲区大小
		m_writePosition = 0;		   	//存储缓冲区当前的写位置
		m_errorCode = 0;														//复位错误代码
	}
}

//******************
//说明：缓冲区类析构函数
//功能：释放给缓冲区指针变量所分配空间
//备注：无
//******************
Buffer::~Buffer()
{
//删除缓冲区
	if(NULL != m_buffer) delete []m_buffer;
	m_buffer = NULL;
}

//******************
//说明：写缓冲区函数
//功能：将src参数指向的内存空间的size字节的数据写入缓冲区对象
//备注：写入成功返回true,不成功返回false
//******************
bool Buffer::WriteBuffer(const char *src, unsigned short size)
{
	if((NULL != m_buffer)&&(m_bufSize >= size))
	{
		memcpy((char*)m_buffer, (char*)src, size);				//将数据拷贝到缓冲区
		m_writePosition = size;														//存储数据内容的大小
		m_errorCode = 0;																	//复位错误代码
		return true;
	}	
	else
	{
		m_errorCode = USR_WRITEBUFFER_OVERFLOW_ERROR;			//存储错误代码
		return false;
	}
}

//******************
//说明：读缓冲区函数
//功能：将指向缓冲区对象的size字节的数据的指针返回
//备注：无
//******************
char *Buffer::ReadBuffer(unsigned short size)
{
    char *des = new char[size];  //(char *)malloc(size);										//分配空间

	if(NULL == des) 																		//内存不足
	{
		m_errorCode = USR_NOT_ENOUGH_MEMORY;							//存储错误代码
	}
	else
	{
		des = (char *)memcpy(des, m_buffer, size);				//拷贝数据到要被返回的数据区
		m_errorCode = 0;																	//复位错误代码
	}
	return des;																					//返回指针
}

//******************
//说明：清空缓冲区函数
//功能：释放缓冲区对象内分配的内存空间
//备注：无
//******************
void Buffer::ClearBuffer()
{
	if(NULL != m_buffer) 																//如果缓冲区不为空
		{
		delete m_buffer;																	//释放缓冲区
		m_buffer = NULL;																	//缓冲区指针置为空
	}
	m_writePosition = 0;		   													//内容的大小清零
	m_bufSize = 0;																			//缓冲区的大小清零
	m_errorCode = 0;																		//复位错误代码
}

//******************
//说明：获取缓冲区内存空间指针函数
//功能：将缓冲区对象内分配的内存空间的指针返回
//备注：无
//******************
char* Buffer::GetBufAddr()
{
	return m_buffer;
}

//******************
//说明：判断缓冲区是否为空函数
//功能：用来判断缓冲区是否为空.
//备注：为空返回true,不为空返回false
//******************
bool Buffer::IsEmpty()
{
	if((NULL == m_buffer)&&(m_writePosition == 0)) return true;
	return false;
}

//******************
//说明：获取缓冲区的大小的函数
//功能：用来获取缓冲区的尺寸.
//备注：返回缓冲区的大小
//******************
unsigned short Buffer::GetBufSize()
{
	return m_bufSize;
}

//******************
//说明：获取缓冲区内写入的数据块的大小的函数
//功能：用来获取缓冲区内写入的数据块的尺寸.
//备注：返回缓冲区写入的数据块的大小
//******************
unsigned short Buffer::GetWritePos()
{
	return m_writePosition;
}

//******************
//说明：缓冲区类的赋值构造函数
//功能：通过赋值操作初始化一个缓冲区实例
//备注：无
//******************
Buffer & Buffer::operator =(const Buffer &a)
{
	if(this == &a) return *this;									//如果是自赋值，返回自身
	if(NULL != m_buffer) delete m_buffer;					//释放缓冲区
	if((0 == a.m_bufSize)&&(NULL == a.m_buffer))	//如果目标缓冲区为空
	{
		m_buffer = NULL; 														//自身缓冲区指针置为空
		m_writePosition = 0;		   									//内容大小清零
		m_bufSize = 0;															//缓冲区大小清零
		m_errorCode = 0;														//复位错误代码
		return *this;	
	}
	m_buffer = new char[a.m_bufSize];							//分配空间

	if(NULL == m_buffer) 													//内存不足
	{
		m_errorCode = USR_NOT_ENOUGH_MEMORY;				//存储错误代码
		return *this;
	}
	memcpy(m_buffer, a.m_buffer, a.m_bufSize);		//拷贝目标缓冲区的内容
	m_writePosition = a.m_writePosition;		   		//存储缓冲区当前的写位置
	m_bufSize = a.m_bufSize;											//拷贝目标的缓冲区的大小
	
	m_errorCode = 0;															//复位错误代码
	return *this;
}



























