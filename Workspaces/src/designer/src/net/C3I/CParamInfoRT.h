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
    char* SetParamValue(unsigned short usParamTable,
        unsigned short usParamCode, char* pRevMem,
        unsigned long ulParamTime, unsigned long ulParamDate);

    //参数值的存放地址
    char* GetParamValue(unsigned short usParamTable, unsigned short usParamCode);

    //参数值的时间
    unsigned long GetParamTime(unsigned short usParamTable, unsigned short usParamCode);

    //输入点火时刻、起飞时刻参数
    void SetXD_FXTime(unsigned short usXDTimeTable, unsigned short usXDTimeCode,
        unsigned short usFXTimeTable, unsigned short usFXTimeCode);
    //设置闰秒
    void SetGPSLeapSecond(UTCTime);
protected:

    //处理0：char和1：unsigned char类型，char、unsigned char可以转换为：
    //
    //0:char、1:unsigned char，12:""	相当于0:char，1:unsigned char，
    //	
    //10:String＆11:Code以及其它无法识别的配置都是错误的配置，
    //	
    //以上都作为默认处理，乘以量纲后使用char或unsigned char存储由b_char
    //ORBYTE决定。	
    //
    //2:short、3:WORD 
    //、4:long、5:DWORD、6:float、7:double、8:PMTime、9:Data、
    //乘以量纲后使用各自类型存储。
    char* InfoCharBYTE(stru_Param *pParam, char *pRevMem, bool b_charORBYTE);
    //处理2：short和3：WORD类型。
    //short、WORD可以转换为：
    //2:short、3:WORD，12:""	相当于2:short，3:WORD，
    //
    //10:String＆11:Code＆0:char＆1:unsigned char以及其它无法识别的配置
    //都是错误的配置，以上都作为默认处理，乘以量纲后使用short
    //或WORD存储由b_shortORWORD决定。
    //	
    //4:long、5:DWORD、6:float、7:double、8:PMTime、9:Data、乘
    //以量纲后使用各自类型存储。
    char* InfoShortWord(stru_Param *pParam, char *pRevMem, bool b_shortORWORD);
    //处理4：long和5：DWORD类型。
    //long、DWORD可以转换为：
    //4:long、5DWORD，12:""	相当于0:char，1:unsigned char，
    //
    //10:String＆11:Code＆0:char＆1:unsigned char＆2:short＆3:WORD 	
    //以及其它无法识别的配置都是错误的配置，以上都作为默认处理
    //，乘以量纲后使用long、DWORD存储由b_longORDWORD决定。
    //	
    //6:float、7:double、8:PMTime、9:Data、乘以量纲后使用各自
    //类型存储。
// 	char* InfoLongDwordE(stru_Param *pParam, char *pRevMem,bool b_longORDWORD);
    char* InfoLongDword(stru_Param *pParam, char *pRevMem, bool b_longORDWORD);
    //处理6：float类型。
    //float可以转换为：
    //6:float、12:""	相当于6:float
    //
    //10:String＆11:Code＆0:char＆1:unsigned char＆2:short＆3:WORD 
    //4long、5DWORD	
    //以及其它无法识别的配置都是错误的配置，以上都作为默认处理
    //，乘以量纲后使用float存储。
    //	
    //7:double、8:PMTime、9:Data、乘以量纲后使用各自类型存储。
    char* InfoFloat(stru_Param *pParam, char *pRevMem);
    //处理7：double类型。
    //7:double可以转换为：
    //7:double、12:""	相当于7:double、，
    //
    //10:String＆11:Code＆0:char＆1:unsigned char＆2:short＆3:WORD 
    //4long、5DWORD、6float	
    //以及其它无法识别的配置都是错误的配置，以上都作为默认处理
    //，乘以量纲后使用7:double存储。
    //	
    //8:PMTime、9:Data、乘以量纲后使用各自类型存储。
    char* InfoDouble(stru_Param *pParam, char *pRevMem);
    //处理9：PMTime、10：Date类型。
    //8:PMTime和9:Date:是一个0.1ms的整数，不需要计算。
    char* InfoPMTimeDate(stru_Param *pParam, char *pRevMem);
    //处理11：String类型。
    //10:String按格式处理
    //字符串数据由"字符串长度"、"字符串有效数据区"和"字符串结
    //束尾"三部分构成。"字符串长度"占2个字节，表示"字符串有效
    //数据区"的长度，不包括本身和字符串结束尾，采用2字节无符号
    //整数表示取值范围1～4000；"字符串有效数据区"存储有效字符
    //，其长度由"字符串长度"决定，采用ANSI（英文字符采用ASCII
    //编码，中文字符采用GB2312编码）编码；"字符串结束尾"为1个
    //字节00h，"字符串有效数据区"中不得出现00f字符。
    char* InfoString(stru_Param *pParam, char *pRevMem);
    //处理11：Code类型。
    //原码数据由"原码长度"、"原码有效数据区"和"原码结束尾"三部
    //分构成。"原码长度"占2个字节，表示"原码有效数据区"的长度
    //，不包括本身和字符串结束尾，采用2字节无符号整数表示，取
    //值范围1～4000；"原码有效数据区"存储有效的二进制原码，其
    //长度由"原码长度"决定；"原码结束尾"为1个字节ffh。
    char* InfoCode(stru_Param *pParam, char *pRevMem);

    //处理12：BCDTime类型。
    char* InfoBCDTime(stru_Param *pParam, char *pRevMem);

    //处理13：UTCTime类型。
    char* InfoUTCTime(stru_Param *pParam, char *pRevMem);

    //处理tp_ulong8,tp_long8类型
    char* InfoLong8(stru_Param *pParam, char *pRevMem, bool b_longORDWORD);

    bool m_bCritical;
    //闰秒
    static UTCTime m_GPSLeapSecond;
    stru_Param* m_pXDTime;//点火时刻
    stru_Param* m_pFXTime;//起飞时刻
    stru_Param* m_pFXTime_Mode;//起飞时刻模式
public:
    //获取闰秒
    static UTCTime GetGPSLeapSecond() { return m_GPSLeapSecond; };
};

//表示时间的结构
struct DateUnit
{
    long time;
    double value;
    DateUnit() :time(0xffffffff) {};
};