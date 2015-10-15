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
#include "selfshare/src/config/config.h"
#include "SimpleLogger.h"

//外部变量.配置信息和缓存信息
extern Config g_cfg;
extern SimpleLogger logger;

//析构函数中释放C3I参数约定表map，同时释放参数所占用的内存空间。
CParamInfoRT::~CParamInfoRT()
{

}

//构造函数。
CParamInfoRT::CParamInfoRT()
{

}

//将接收到的网络数据帧中的参数的值加以处理并存入指定的表号
//、字段号的参数地址中。
char* CParamInfoRT::SetParamValue(AbstractParam* pParam,
								  char* pRevMem,
                                  unsigned int ulParamTime,
                                  unsigned int ulParamDate)
{		
	//接收缓冲区是空的
	if( pRevMem == 0 )
		return 0;//直接返回错误
    if( pParam == NULL )//没有该参数
		return 0;//直接返回错误
    pParam->SetParamDate(ulParamDate);
    pParam->SetParamTime(ulParamTime);
    char* pp = NULL;
    switch(pParam->GetParamTranType())
	{
    case tp_char:
    case tp_BYTE:
    case tp_short:
    case tp_WORD:
    case tp_long:
    case tp_DWORD:
    case tp_float:
    case tp_double:
    case tp_PMTime:
    case tp_Date:
    case tp_BCDTime:
    case tp_UTCTime:
    case tp_ulong8:
    case tp_long8:
    {
        memcpy((char*)(pParam->GetParamValuePoint()),pRevMem,pParam->GetParamDataLen());
        pRevMem += pParam->GetParamDataLen();
        pp = pRevMem;
        double t_value = 0.0;
        double old_value = 0.0;
        if(pParam->getValue(old_value))
        {
            if (pParam->getValueFromCode(t_value))
            {
                //比较
                if (pParam->GetParamStatus() == AbstractParam::First)
                {
                    pParam->SetParamStatus(AbstractParam::New);
                }
                else
                {
                    if (old_value == t_value)
                    {
                        pParam->SetParamStatus(AbstractParam::Old);
                    }
                    else
                    {
                        pParam->SetParamStatus(AbstractParam::New);
                    }
                }
                HistoryParam hParam;
                hParam.setDate(pParam->GetParamDate());
                hParam.setTime(pParam->GetParamTime());
                hParam.setValue(t_value);
                g_cfg.m_zxHistoryParamBuf.PutBuffer(pParam->GetTableNo(), pParam->GetParamNo(), hParam);
                logger.logPacketCount();
            }
        }
        break;
    }
	case 10://String
		pp = InfoString(pParam, pRevMem);//String
		break;
	case 11://Code
		pp = InfoCode(pParam, pRevMem);//Code
		break;
	//数据传输类型不能是其它形式，否则返回错误
	default:pp = 0;
		break;
	}
	return pp;
}
//处理11：String类型。
//10:String按格式处理
//字符串数据由"字符串长度"、"字符串有效数据区"和"字符串结束尾"三部分构成。"字符串
//长度"占2个字节，表示"字符串有效数据区"的长度，不包括本身和字符串结束尾，采用2字
//节无符号整数表示取值范围1～4000；"字符串有效数据区"存储有效字符，其长度由"字符串
//长度"决定，采用ANSI（英文字符采用ASCII编码，中文字符采用GB2312编码）编码；"字符
//串结束尾"为1个字节00h，"字符串有效数据区"中不得出现00f字符。
char* CParamInfoRT::InfoString(AbstractParam *pParam, char *pRevMem)
{
	//字符串长度
	unsigned short StrLen = *(unsigned short*)pRevMem;
	if( StrLen >4000 )//字符串过长
		return 0;
	//字符串结束尾
	unsigned char StrEnd = *(unsigned char*)(pRevMem+2+StrLen);
	if( StrEnd != 0 )//字符串结束尾不是0
		return 0;
	//取出,把尾也取出来
    memcpy((char*)(pParam->GetParamValuePoint()),pRevMem+2,StrLen+1);
    pParam->SetParamDataLen(StrLen+1);
	pRevMem+=(StrLen+3);//移动地址StrLen+3个字节
	return pRevMem;
}

//处理11：Code类型。
//原码数据由"原码长度"、"原码有效数据区"和"原码结束尾"三部分构成。"原码长度"占2个
//字节，表示"原码有效数据区"的长度，不包括本身和字符串结束尾，采用2字节无符号整数
//表示，取值范围1～4000；"原码有效数据区"存储有效的二进制原码，其长度由"原码长度"
//决定；"原码结束尾"为1个字节ffh。
char* CParamInfoRT::InfoCode(AbstractParam *pParam, char *pRevMem)
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
    memcpy((char*)(pParam->GetParamValuePoint()),pRevMem+2,CodeLen);
    pParam->SetParamDataLen(CodeLen);
	pRevMem+=(CodeLen+3);//移动地址CodeLen+1个字节
	return pRevMem;
}
