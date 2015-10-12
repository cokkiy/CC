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
// 

#include "CParamInfoRT.h"
//#include "CAxis.h"
//#include "HistoryBuffer.h"
//#include "XMLReadT0List.h"
//#include "CXMLReaderC3IParam.h"
#include "stru.h"
#include "selfshare/src/datastruct.h"
#include "selfshare/src/config/config.h"
#include "SimpleLogger.h"

//外部变量.配置信息和缓存信息
extern Config g_cfg;
//外部变量,数据记录工具
extern SimpleLogger logger;

UTCTime CParamInfoRT::m_GPSLeapSecond = 15.0;

//析构函数中释放C3I参数约定表map，同时释放参数所占用的内存空间。
CParamInfoRT::~CParamInfoRT()
{
	//析构对象时，释放参数map及参数的地址空间。
	//this->ClearParam();
}

//构造函数。
CParamInfoRT::CParamInfoRT()
{
	m_pXDTime = NULL;//点火时刻
	m_pFXTime = NULL;//起飞时刻
	m_pFXTime_Mode = NULL;
	m_bCritical = false;
	//m_pFile = NULL;
}

//将接收到的网络数据帧中的参数的值加以处理并存入指定的表号
//、字段号的参数地址中。
char* CParamInfoRT::SetParamValue(unsigned short usParamTable, 
								  unsigned short usParamCode, 
								  char* pRevMem,
								  unsigned long ulParamTime,
								  unsigned long ulParamDate)
{		
	//接收缓冲区是空的
	if( pRevMem == 0 )
		return 0;//直接返回错误

	//为了解决东风中心发现的BUG：
	//T0时间为0，T0模式为0的情况下，
	//在某些指显终端上偶尔出现，
	//T0时间闪烁显示“00时00分00秒000毫秒”的现象
	//新增T0时间临时存放参数的位置
	//刘裕贵、陶小勇，2013年10月22日
	//取T0参数的临时地址	
    stru_Param* pParam = NULL;  //即将处理生产的参数
	//如果没有取到T0参数的临时地址，则取参数的地址。
	if (pParam==NULL)
	{
		pParam = (stru_Param*)GetParam(usParamTable, usParamCode);	
	}
	if( pParam == 0 )//没有该参数
		return 0;//直接返回错误
	pParam->ulParamTime = ulParamTime;
	pParam->ulParamDate = ulParamDate;

	
/************************************************************
'	created:	30:12:2012   16:07
'	author:		刘裕贵
'	
'	purpose:	初始化，参数默认不设置颜色
************************************************************/
	char* pp = NULL;
	switch(pParam->ucTransType)
	{
	/*	0:char;1:unsigned char;2:short;3:WORD;4:long;
 		5:DWORD;6:float;7:double;8:PMTime;
 		9:Data;10:String;11:Code;12:"";*/
	case 0://char
		pp = InfoCharBYTE(pParam, pRevMem,0);//char
		break;
	case 1://unsigned char
		pp = InfoCharBYTE(pParam, pRevMem,1);//unsigned char
		break;
	case 2://short
		pp = InfoShortWord(pParam, pRevMem,0);//short
		break;
	case 3://WORD
		pp = InfoShortWord(pParam, pRevMem,1);//WORD
		break;
	case 4://long
		pp = InfoLongDword(pParam, pRevMem,0);//long
		break;
	case 5://DWORD
		pp = InfoLongDword(pParam, pRevMem,1);//DWORD
		break;
	case 6://float
		pp = InfoFloat(pParam, pRevMem);//float
		break;
	case 7://double
		pp = InfoDouble(pParam, pRevMem);//double
		break;
	case 8://PMTime
		pp = InfoPMTimeDate(pParam, pRevMem);//PMTime
		break;
	case 9://Data
		pp = InfoPMTimeDate(pParam, pRevMem);//Data
		break;
	case 10://String
		pp = InfoString(pParam, pRevMem);//String
		break;
	case 11://Code
		pp = InfoCode(pParam, pRevMem);//Code
		break;
	case tp_BCDTime://BCDTime
		pp = InfoBCDTime(pParam, pRevMem);//BCDTime
		break;
	case tp_UTCTime://UTCTime
		pp = InfoUTCTime(pParam, pRevMem);//UTCTime
		break;
	case tp_ulong8:
		pp = InfoLong8(pParam, pRevMem,1);
		break;
	case tp_long8:
		pp = InfoLong8(pParam, pRevMem,0);
		break;
// 	case tp_longE://
// 		pp = InfoLongDwordE(pParam, pRevMem,0);//long
// 		break;
	//数据传输类型不能是其它形式，否则返回错误
	default:pp = 0;
		break;
	}
    AbstractParam abstractParam = (AbstractParam)(*pParam);
    
    // added by cokkiy, 存入到数据缓冲区中
    g_cfg.m_zxParamBuf.PutBuffer(abstractParam);

    //记录日志
    //logger.log(*pParam);   
    logger.logPacketCount();

    HistoryParam hParam;
    double t_value = 0.0;
    if (abstractParam.getValueFromCode(t_value))
    {
        //hParam.setDate(param.GetParamTime());
        hParam.setTime(abstractParam.GetParamTime());
        hParam.setValue(t_value);
        g_cfg.m_zxHistoryParamBuf.PutBuffer(abstractParam.GetTableNo(), abstractParam.GetParamNo(), hParam);
    }

	m_bCritical = false;
	return pp;
}

//处理0：char和1：unsigned char类型，char、unsigned char可以转换为：
//
//0:char、1:unsigned char，12:""	相当于0:char，1:unsigned char，
//	
//10:String＆11:Code以及其它无法识别的配置都是错误的配置，	
//以上都作为默认处理，乘以量纲后使用char或unsigned char存储由b_charORBYTE决定。	
//
//2:short、3:WORD 
//、4:long、5:DWORD、6:float、7:double、8:PMTime、9:Data、乘以量纲后使用各自类型存
//储。
char* CParamInfoRT::InfoCharBYTE(stru_Param *pParam, char *pRevMem,bool b_charORBYTE)
{
	//比较
	if (pParam->ucNew == 0)
	{
		pParam->ucNew = 2;
	} 
	else
	{
		if (memcmp( (char*)(pParam->ucParamValue_Old), pRevMem, 1 ) == 0)
		{
			pParam->ucNew = 1;
		}
		else
		{
			pParam->ucNew = 2;
		}
	}
	
	//char一个字节
	memcpy((char*)(pParam->pParamValue),pRevMem,1);	
	memcpy((char*)(pParam->ucParamValue_Old),pRevMem,1);	
	return ++pRevMem;
}

//处理2：short和3：WORD类型。
//short、WORD可以转换为：
//2:short、3:WORD，12:""	相当于2:short，3:WORD，
//
//10:String＆11:Code＆0:char＆1:unsigned char以及其它无法识别的配置都是错误的配置，以上都作
//为默认处理，乘以量纲后使用short或WORD存储由b_shortORWORD决定。
//	
//4:long、5:DWORD、6:float、7:double、8:PMTime、9:Data、乘以量纲后使用各自类型存储
//。
char* CParamInfoRT::InfoShortWord(stru_Param *pParam, char *pRevMem,bool b_shortORWORD)
{
	//比较
	if (pParam->ucNew == 0)
	{
		pParam->ucNew = 2;
	} 
	else
	{
		if (memcmp( (char*)(pParam->ucParamValue_Old), pRevMem, 2 ) == 0)
		{
			pParam->ucNew = 1;
		}
		else
		{
			pParam->ucNew = 2;
		}
	}
	//char一个字节
	memcpy((char*)(pParam->pParamValue),pRevMem,2);	
	memcpy((char*)(pParam->ucParamValue_Old),pRevMem,2);	
	return pRevMem+2;
}
//处理4：long和5：DWORD类型。
//long、DWORD可以转换为：
//4:long、5DWORD，12:""	相当于0:char，1:unsigned char，
//
//10:String＆11:Code＆0:char＆1:unsigned char＆2:short＆3:WORD 	
//以及其它无法识别的配置都是错误的配置，以上都作为默认处理，乘以量纲后使用long、DW
//ORD存储由b_longORDWORD决定。
//	
//6:float、7:double、8:PMTime、9:Data、乘以量纲后使用各自类型存储。
char* CParamInfoRT::InfoLongDword(stru_Param *pParam, char *pRevMem,bool b_longORDWORD)
{
	//比较
	if (pParam->ucNew == 0)
	{
		pParam->ucNew = 2;
	} 
	else
	{
		if (memcmp( (char*)(pParam->ucParamValue_Old), pRevMem, 4 ) == 0)
		{
			pParam->ucNew = 1;
		}
		else
		{
			pParam->ucNew = 2;
		}
	}
	//char一个字节
	memcpy((char*)(pParam->pParamValue),pRevMem,4);	
	memcpy((char*)(pParam->ucParamValue_Old),pRevMem,4);	
	return pRevMem+4;
}
char* CParamInfoRT::InfoLong8(stru_Param *pParam, char *pRevMem,bool b_longORDWORD)
{
	//比较
	if (pParam->ucNew == 0)
	{
		pParam->ucNew = 2;
	} 
	else
	{
		if (memcmp( (char*)(pParam->ucParamValue_Old), pRevMem, 8 ) == 0)
		{
			pParam->ucNew = 1;
		}
		else
		{
			pParam->ucNew = 2;
		}
	}
	//char一个字节
	memcpy((char*)(pParam->pParamValue),pRevMem,8);	
	memcpy((char*)(pParam->ucParamValue_Old),pRevMem,8);	
	return pRevMem+8;
}

//7:double、8:PMTime、9:Data、乘以量纲后使用各自类型存储。
char* CParamInfoRT::InfoFloat(stru_Param *pParam, char *pRevMem)
{
	//比较
	if (pParam->ucNew == 0)
	{
		pParam->ucNew = 2;
	} 
	else
	{
		if (memcmp( (char*)(pParam->ucParamValue_Old), pRevMem, 4 ) == 0)
		{
			pParam->ucNew = 1;
		}
		else
		{
			pParam->ucNew = 2;
		}
	}
	//char一个字节
	memcpy((char*)(pParam->pParamValue),pRevMem,4);	
	memcpy((char*)(pParam->ucParamValue_Old),pRevMem,4);	
	return pRevMem+4;
}

//处理7：double类型。
//7:double可以转换为：
//7:double、12:""	相当于7:double、，
//
//10:String＆11:Code＆0:char＆1:unsigned char＆2:short＆3:WORD 4long、5DWORD、6float	
//以及其它无法识别的配置都是错误的配置，以上都作为默认处理，乘以量纲后使用7:double
//存储。
//	
//8:PMTime、9:Data、乘以量纲后使用各自类型存储。
char* CParamInfoRT::InfoDouble(stru_Param *pParam, char *pRevMem)
{
	//比较
	if (pParam->ucNew == 0)
	{
		pParam->ucNew = 2;
	} 
	else
	{
		if (memcmp( (char*)(pParam->ucParamValue_Old), pRevMem, 8 ) == 0)
		{
			pParam->ucNew = 1;
		}
		else
		{
			pParam->ucNew = 2;
		}
	}
	//char一个字节
	memcpy((char*)(pParam->pParamValue),pRevMem,8);	
	memcpy((char*)(pParam->ucParamValue_Old),pRevMem,8);	
	return pRevMem+8;
}

//处理9：PMTime、10：Date类型。
//8:PMTime和9:Date:是一个0.1ms的整数，不需要计算。
char* CParamInfoRT::InfoPMTimeDate(stru_Param *pParam, char *pRevMem)
{
	//比较
	if (pParam->ucNew == 0)
	{
		pParam->ucNew = 2;
	} 
	else
	{
		if (memcmp( (char*)(pParam->ucParamValue_Old), pRevMem, 4 ) == 0)
		{
			pParam->ucNew = 1;
		}
		else
		{
			pParam->ucNew = 2;
		}
	}
	memcpy((char*)(pParam->pParamValue),pRevMem,4);	
	memcpy((char*)(pParam->ucParamValue_Old),pRevMem,4);	
	return pRevMem+4;
}

//处理11：String类型。
//10:String按格式处理
//字符串数据由"字符串长度"、"字符串有效数据区"和"字符串结束尾"三部分构成。"字符串
//长度"占2个字节，表示"字符串有效数据区"的长度，不包括本身和字符串结束尾，采用2字
//节无符号整数表示取值范围1～4000；"字符串有效数据区"存储有效字符，其长度由"字符串
//长度"决定，采用ANSI（英文字符采用ASCII编码，中文字符采用GB2312编码）编码；"字符
//串结束尾"为1个字节00h，"字符串有效数据区"中不得出现00f字符。
char* CParamInfoRT::InfoString(stru_Param *pParam, char *pRevMem)
{
	//字符串长度
	unsigned short StrLen = *(unsigned short*)pRevMem;
	if( StrLen >4000 )//字符串过长
		return 0;
//	pRevMem+=2;//移动地址2个字节
	//字符串结束尾
	unsigned char StrEnd = *(unsigned char*)(pRevMem+2+StrLen);
	if( StrEnd != 0 )//字符串结束尾不是0
		return 0;
	//取出,把尾也取出来
	memcpy((char*)(pParam->pParamValue),pRevMem+2,StrLen+1);
	pParam->usParamLen = StrLen+1;
	pRevMem+=(StrLen+3);//移动地址StrLen+3个字节
	return pRevMem;
}

//处理11：Code类型。
//原码数据由"原码长度"、"原码有效数据区"和"原码结束尾"三部分构成。"原码长度"占2个
//字节，表示"原码有效数据区"的长度，不包括本身和字符串结束尾，采用2字节无符号整数
//表示，取值范围1～4000；"原码有效数据区"存储有效的二进制原码，其长度由"原码长度"
//决定；"原码结束尾"为1个字节ffh。
char* CParamInfoRT::InfoCode(stru_Param *pParam, char *pRevMem)
{	
	//原码长度
	unsigned short CodeLen = *(unsigned short*)pRevMem;
	if( CodeLen < 1 || CodeLen >4000 )//原码过长
		return 0;
//	pRevMem+=2;//移动地址2个字节
	//原码结束尾
	unsigned char CodeEnd = *(unsigned char*)(pRevMem+2+CodeLen);
	if( CodeEnd != 0xff )//原码结束尾不是0xff
		return 0;
	//取出
	memcpy((char*)(pParam->pParamValue),pRevMem+2,CodeLen);	
	pParam->usParamLen = CodeLen;
	pRevMem+=(CodeLen+3);//移动地址CodeLen+1个字节
	return pRevMem;
}

//参数值的存放地址
char* CParamInfoRT::GetParamValue(unsigned short usParamTable, unsigned short usParamCode)
{
	//取参数地址
	stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
	if(param != 0)//该参数存在
		return param->pParamValue;
	else//该参数不存在返回0
		return 0;
}

//参数值的时间
unsigned long CParamInfoRT::GetParamTime(unsigned short usParamTable, unsigned short usParamCode)
{
	//取参数地址
	stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
	if(param != 0)//该参数存在
		return param->ulParamTime;
	else//该参数不存在返回0
		return 0xffffffff;
}

void CParamInfoRT::SetXD_FXTime( unsigned short usXDTimeTable, unsigned short usXDTimeCode, unsigned short usFXTimeTable, unsigned short usFXTimeCode )
{
	m_pXDTime = (stru_Param *)GetParam(usXDTimeTable, usXDTimeCode);//点火时刻
	m_pFXTime = (stru_Param *)GetParam(usFXTimeTable, usFXTimeCode);//起飞时刻
	m_pFXTime_Mode = (stru_Param *)GetParam(usFXTimeTable, usFXTimeCode+1);//起飞时刻模式
}
void CParamInfoRT::SetGPSLeapSecond(UTCTime LeapSecond)
{
	m_GPSLeapSecond = LeapSecond;
}

//处理12：BCDTime类型。
char* CParamInfoRT::InfoBCDTime( stru_Param *pParam, char *pRevMem )
{
	//比较
	if (pParam->ucNew == 0)
	{
		pParam->ucNew = 2;
	} 
	else
	{
		if (memcmp( (char*)(pParam->ucParamValue_Old), pRevMem, 4 ) == 0)
		{
			pParam->ucNew = 1;
		}
		else
		{
			pParam->ucNew = 2;
		}
	}
	
	memcpy((char*)(pParam->pParamValue),pRevMem,4);	
	memcpy((char*)(pParam->ucParamValue_Old),pRevMem,4);
    return pRevMem+4;
}

//处理13：UTCTime类型。
char* CParamInfoRT::InfoUTCTime( stru_Param *pParam, char *pRevMem )
{
	//比较
	if (pParam->ucNew == 0)
	{
		pParam->ucNew = 2;
	} 
	else
	{
		if (memcmp( (char*)(pParam->ucParamValue_Old), pRevMem, 4 ) == 0)
		{
			pParam->ucNew = 1;
		}
		else
		{
			pParam->ucNew = 2;
		}
	}
	memcpy((char*)(pParam->pParamValue),pRevMem,4);	
	memcpy((char*)(pParam->ucParamValue_Old),pRevMem,4);
	return pRevMem+4;
}


