// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：parameter.h
// 文件摘要：综合应用分系统参数类头文件，用来声明参数类。
// 
// 原始版本：1.0
// 当前版本：1.1(不对参数进行处理)
// 作    者：肖胜杰
// 完成日期：

#ifndef PARAMETER_H
#define PARAMETER_H

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<list>
#include "../../../net_global.h"
#include "../../../selfshare/src/baseobject.h"
#include "../../../selfshare/src/const.h"
#include "../../../selfshare/src/datastruct.h"

//#include "../infotable/infotable.h"

using namespace std;

class NETSHARED_EXPORT Parameter:public BaseObject
{
	public:
		Parameter();																								//构造函数
		Parameter(const Parameter &a);															//拷贝构造函数
		Parameter & operator =(const Parameter &a);									// 赋值函数
		~Parameter();																								//析构函数
		//void InputInfoTable(InfoTableProc *tabproc);				//输入信息约定表	
		bool GetParamInfo(unsigned short tableno, unsigned short paramno);
		bool GetNormalParamInfo(unsigned short tableno, unsigned short paramno);		//获取公用处理信息
		bool SetParamVal(const void *val);													//设置参数的值
		unsigned short GetTableNo();												//获取表号
		unsigned short GetParamNo();												//获取参数号
		unsigned short GetNextParamNo();												//获取下一个参数号
		//unsigned char GetParamProcFlag();												//获取参数辅助决策标志
		string GetParamDataType();												//获取参数数据类型
		unsigned short GetParamDataLen();												//获取参数数据长度
		string GetParamTitle();																	//获取参数标题
		bool GetFrameData(void *des, int flag = 0);				//当flag = 0时,只返回参数值,当flag = 1时,返回参数号和参数值,当flag = 2 时,返回参数有效标志和参数值
		void FreeParamSpace();														//释放参数所占用的空间
		void SetParamTime(unsigned int time);							//设置参数采集时间
		void SetParamLen(unsigned short len);							//设置参数实际长度(对string和code型参数使用)
		unsigned int GetParamTime();                      //返回参数采集时间
        void SetParamVaildFlag(char flag);
		string GetParamValDisp();					// 转换参数值为显示模式函数
		char GetHighChar(unsigned char c);					// 转换十六进制数为字符串函数  高位
		char GetLowChar(unsigned char c);					// 转换十六进制数为字符串函数  低位

		//WholeTable    *m_infoTab;           //公用信息约定表
		
		unsigned short m_tableNo;  				 //表号
		unsigned short m_paramNo;          //参数号
		unsigned short m_nextParamNo;      //下一个参数号，用于在解完整信息帧时避免每一个参数查两次表
		string         m_paramTitle;       //参数标题
		string         m_paramDataType;    //参数数据类型
		unsigned short m_paramDataLen;     	//参数数据长度
        char           m_paramVaildFlag;
		void          *m_paramVal;	 			 //参数初值
		unsigned int   m_paramTime;        //参数采集时间,绝对时
						
};

#endif

