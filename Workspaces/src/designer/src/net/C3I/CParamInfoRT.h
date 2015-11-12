// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：
// 文件摘要： 
//  
// 原始版本：
// 作    者：
// 完成日期：
//  
// *************************** 修改记录 ************************** //
// 版    本：
// 修 改 者：
// 完成日期：
// 修改内容：
// 
// *************************************************************** //

#pragma once

#include "CParamInfo.h"

//该类继承CParamInfo用于对C3I/PDXP参数的处理，在进程初始化时利用该
//类对象装在参数进内存，当实时任务时将接收网络网络信息帧的参数
//值存储指定的参数地址中，通过该类可以获取某一表号、字段号参数
//的所有信息。
class  CParamInfoRT : public CParamInfo
{
public:
	//析构函数中释放C3I参数约定表map，同时释放参数所占用的内存
	//空间。
	virtual ~CParamInfoRT();

	//构造函数。
	CParamInfoRT();

	//CStdioFile* m_pFile; 
	//将接收到的网络数据帧中的参数的值加以处理并存入指定的表号
	//、字段号的参数地址中。
    char* SetParamValue(AbstractParam* pParam, char* pRevMem,
        unsigned int ulParamTime,unsigned int ulParamDate);

    /*!
    T0清屏,置beginTabNo和endTableNo之间的所有参数时间为0xFFFFFFFF
    @param unsigned short beginTabNo 开始表号
    @param unsigned short endTableNo 结束表号
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/12 10:13:58
    */
    void clear(unsigned short beginTabNo, unsigned short endTableNo);
protected:	
	//处理11：String类型。
	//10:String按格式处理
	//字符串数据由"字符串长度"、"字符串有效数据区"和"字符串结
	//束尾"三部分构成。"字符串长度"占2个字节，表示"字符串有效
	//数据区"的长度，不包括本身和字符串结束尾，采用2字节无符号
	//整数表示取值范围1～4000；"字符串有效数据区"存储有效字符
	//，其长度由"字符串长度"决定，采用ANSI（英文字符采用ASCII
	//编码，中文字符采用GB2312编码）编码；"字符串结束尾"为1个
	//字节00h，"字符串有效数据区"中不得出现00f字符。
    char* InfoString(AbstractParam *pParam, char *pRevMem);
	//处理11：Code类型。
	//原码数据由"原码长度"、"原码有效数据区"和"原码结束尾"三部
	//分构成。"原码长度"占2个字节，表示"原码有效数据区"的长度
	//，不包括本身和字符串结束尾，采用2字节无符号整数表示，取
	//值范围1～4000；"原码有效数据区"存储有效的二进制原码，其
	//长度由"原码长度"决定；"原码结束尾"为1个字节ffh。
    char* InfoCode(AbstractParam *pParam, char *pRevMem);
};
