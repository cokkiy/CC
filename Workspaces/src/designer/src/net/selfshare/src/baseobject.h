// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：baseobject.h
// 文件摘要：综合应用分系统所有类的夫类头文件，错误代码的声明和处理。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef BASEOBJECT_H
#define BASEOBJECT_H

class BaseObject
{
	public:
		BaseObject():m_errorCode(0){};																																				//构造函数
		
		//*****************
		//名称：获取错误代码函数
		//功能：获取错误代码
		//备注：无
		//*****************
		unsigned short GetErrorCode(){unsigned short temp = m_errorCode; m_errorCode = 0; return(temp);}			//获取错误代码

	protected:
		unsigned short m_errorCode;        //错误代码
};

#endif

