// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：parameter.cpp
// 文件摘要：综合应用分系统参数类实现文件，用来实现参数类。
// 
// 原始版本：1.0
// 当前版本：1.1(不对参数进行处理)
// 作    者：肖胜杰
// 完成日期：

#include "parameter.h"
#include "../../../selfshare/src/OS_Def.h"
#include "C3I/CParamInfoRT.h"

#ifdef LINUX
#define _strnicmp strncasecmp
#endif

//外部变量,C3I/PDXP包处理对象信息约定表
extern CParamInfoRT g_paramInfoRT;

//*****************************************************************
//构造函数
//
//
//*****************************************************************
Parameter::Parameter():BaseObject()
{
	//m_infoTab = NULL;	 //初始化信息约定表指针
	m_tableNo = 0;															//初始化表号成员变量
	m_paramNo = 0;															//初始化参数号成员变量
	m_paramTitle = "";													//初始化参数标题成员变量
	m_paramDataType = "";												//初始化参数数据类型成员变量
	m_paramDataLen = 0;												  //初始化参数数据长度成员变量
    m_paramVaildFlag = 0;
    m_paramVal = NULL;													//初始化指针成员变量(量纲、参数值)
    m_paramTime = 0xffffffff;														//初识化参数采集时间
	m_nextParamNo = 0;													//初始化下一参数号
}

//*****************************************************************
//拷贝构造函数
//
//
//*****************************************************************
Parameter::Parameter(const Parameter &a):BaseObject()
{
	m_errorCode = 0;
	//m_infoTab = a.m_infoTab;																 //拷贝信息约定表
	m_tableNo = a.m_tableNo;															   //拷贝表号成员变量
	m_paramNo = a.m_paramNo;															   //拷贝参数号成员变量
	m_nextParamNo = a.m_nextParamNo;												 //拷贝下一参数号
	m_paramTitle = a.m_paramTitle;													 //拷贝参数标题成员变量
	m_paramDataType = a.m_paramDataType;										 //拷贝参数数据类型成员变量
    m_paramVaildFlag = a.m_paramVaildFlag;
    m_paramDataLen = a.m_paramDataLen;											 //拷贝参数数据长度成员变量
	m_paramTime = a.m_paramTime;														 //拷贝参数采集时间
	m_errorCode = a.m_errorCode;															 //拷贝错误代码

	if(NULL == a.m_paramVal) m_paramVal = NULL;							//如果参数初值指针为空
	else																										//如果参数初值指针不为空
	{
		m_paramVal = malloc(m_paramDataLen);									//分配空间
		if(NULL == m_paramVal)  														//空间分配失败
		{
			m_errorCode = USR_NOT_ENOUGH_MEMORY;								//填写错误代码
		}
		else
		{
			memcpy(m_paramVal, a.m_paramVal, m_paramDataLen);				//拷贝参数传输值
		}
	}
}

//*****************************************************************
//赋值构造函数
//
//
//*****************************************************************
Parameter & Parameter::operator =(const Parameter &a)
{
	map<unsigned char, int>::iterator theIterator;
	if(this == &a) 																	//如果是自赋值,返回本对象指针
	{
		m_errorCode = 0	;
		return *this;
	}
	if(NULL != m_paramVal) free(m_paramVal);				//释放本对象的参数网络传输值指针的空间
	m_paramDataLen = a.m_paramDataLen;						//取得被赋值对象的数据长度
	
	if(NULL != a.m_paramVal)								//如果被赋值对象的参数网络传输值指针不为空
	{
		m_paramVal = malloc(m_paramDataLen);			//给本对象的变量指针分配空间
		if(NULL == m_paramVal)  								//如果内存不足
		{
			m_errorCode = USR_NOT_ENOUGH_MEMORY;				//存储错误代码
			return *this;
		}
		memcpy(m_paramVal, a.m_paramVal, m_paramDataLen);			//拷贝参数网络传输值
	}
	else m_paramVal = NULL;								//如果被赋值对象的参数网络传输值指针为空.本对象的参数网络传输值指针也置为空
	
	//m_infoTab = a.m_infoTab;																 //拷贝信息约定表
	m_tableNo = a.m_tableNo;															   //拷贝表号成员变量
	m_paramNo = a.m_paramNo;															   //拷贝参数号成员变量
	m_nextParamNo = a.m_nextParamNo;												 //拷贝下一参数号
	m_paramTitle = a.m_paramTitle;													 //拷贝参数标题成员变量
	m_paramDataType = a.m_paramDataType;										 //拷贝参数数据类型成员变量
    m_paramVaildFlag = a.m_paramVaildFlag;
    m_paramDataLen = a.m_paramDataLen;											 //拷贝参数数据长度成员变量
	m_paramTime = a.m_paramTime;														 //拷贝参数采集时间
	m_errorCode = 0;
	return *this;	
}

//*****************************************************************
//根据表号和参数号，
//获取从信息约定表获取参数相关信息，填入参数类的各个成员变量
//返回值为false，原因可能为配置有误，表号不存在，参数号不存在，内存不足
//关于paramno的取值,当paramno为0时，表示是完整信息帧的第一个参数，paramno不为0时，其取值为参数号
//*****************************************************************
bool Parameter::GetParamInfo(unsigned short tableno, unsigned short paramno)
{
	if (GetNormalParamInfo(tableno, paramno) == false)
	{
		return false;
	}

	return true;
}

//*****************************************************************
//根据表号和参数号，
//获取从公用信息约定表获取参数相关信息，填入参数类的各个成员变量
//返回值为false，原因可能为配置有误，表号不存在，参数号不存在，内存不足
//关于paramno的取值,当paramno为0时，表示是完整信息帧的第一个参数，paramno不为0时，其取值为参数号
//*****************************************************************
bool Parameter::GetNormalParamInfo(unsigned short tableno, unsigned short paramno)
{
    //获取信息约定表
    TableMap* tableMap = g_paramInfoRT.getTableMap();
    m_tableNo = tableno;
    if (0 != paramno)
    {
        m_paramNo = paramno;
        auto table=tableMap->find(tableno);
        if (table != tableMap->end()) //找到表号
        {
            //在信息约定表中查找表号对应项
            auto param = table->second->find(paramno);
            if (param != table->second->end())
            {
                //信息约定表中存在指定的表号和参数号
                stru_Param* para = param->second;
                m_paramTitle = para->pcParamName.toStdString();
                //m_pcParamID = para->pcParamID.toStdString();
                m_paramDataType = para->ucTransType;
                m_paramDataLen = para->usParamLen;                
            }
            else 			//没找到参数号对应项，返回false
            {
                m_errorCode = USR_INFOTABLE_PARAMNO_FIND_ERROR;				//存储错误代码
                return false;
            }
        }   
        else       		//没找到表号
        {
            m_errorCode = USR_INFOTABLE_TABLENO_FIND_ERROR;				//存储错误代码
            return false;
        }        
        m_errorCode = 0;
    }  //0 != paramno
    else
    {
        m_paramNo = paramno;
        auto table = tableMap->find(tableno);
        if (table != tableMap->end()) //在信息约定表中查找表号对应项
        {
            if (table->second->begin() != table->second->end())
            {
                //至少有一个参数
                auto param = table->second->begin();
                m_paramNo = (*param).first;
                stru_Param* para = param->second;
                m_paramTitle = para->pcParamName.toStdString();
                //m_pcParamID = para->pcParamID.toStdString();
                m_paramDataType = para->ucTransType;
                m_paramDataLen = para->usParamLen;
            }
            else 			//没找到参数号对应项，返回false
            {
                m_errorCode = USR_INFOTABLE_PARAMNO_FIND_ERROR;				//存储错误代码
                return false;
            }
        }
        else 				//没找到表号
        {
            m_errorCode = USR_INFOTABLE_TABLENO_FIND_ERROR;				//存储错误代码
            return false;
        }
        m_errorCode = 0;
    }

	return true;
}

//*****************************************************************
//获取参数标题
//
//
//*****************************************************************
string Parameter::GetParamTitle()
{
	return(m_paramTitle);
}

void Parameter::SetParamVaildFlag(char flag)
{
    m_paramVaildFlag = flag;
}

//*****************************************************************
//输入信息约定表
//
//
//*****************************************************************
//void Parameter::InputInfoTable(InfoTableProc *tabproc)
//{
//	//m_infoTab = tabproc->GetNormalTab();    //将信息约定表处理类对象赋给类内部成员指针变量
//}

//*****************************************************************
//设置参数值
//
//
//*****************************************************************
bool Parameter::SetParamVal(const void *val)
{
	unsigned short tempParamLen;
	tempParamLen = 0;
	if((0 == _strnicmp(m_paramDataType.c_str(), "string", 6))||(0 == _strnicmp(m_paramDataType.c_str(), "code", 4)))
	{
		memcpy(&tempParamLen, val, 2);							//拷贝参数长
		m_paramDataLen = tempParamLen + 3;							//存储参数长
	}
	if(m_paramDataLen > 0) 
	{
		if(m_paramVal != NULL) free(m_paramVal);
		m_paramVal = NULL;
		m_paramVal = malloc(m_paramDataLen);					//给参数值分配空间
	}
	else 
	{
		m_errorCode = USR_STRING_OR_CODE_ERROR;				//存储错误代码
		return false;
	}
	if(NULL == m_paramVal) 										  	//内存不足
	{
		m_errorCode = USR_NOT_ENOUGH_MEMORY;				//存储错误代码
		return false;
	}
	memcpy(m_paramVal, val, m_paramDataLen);			//拷贝值
	m_errorCode = 0;
	return true;
}

//*****************************************************************
//取得参数的表号
//
//
//*****************************************************************
unsigned short Parameter::GetTableNo()
{
	return m_tableNo;					//返回表号
}

//*****************************************************************
//取得参数的参数号
//
//
//*****************************************************************
unsigned short Parameter::GetNextParamNo()
{
	return m_nextParamNo;					//返回下一个参数号
}

//*****************************************************************
//设置参数采集时间
//
//
//*****************************************************************
void Parameter::SetParamTime(unsigned int time)
{
	m_paramTime = time;
}

//*****************************************************************
//返回参数采集时间
//
//
//*****************************************************************
unsigned int Parameter::GetParamTime()
{
	return m_paramTime;
}

//*****************************************************************
//取得参数的参数号
//
//
//*****************************************************************
unsigned short Parameter::GetParamNo()
{
	return m_paramNo;					//返回参数号
}

//*****************************************************************
//取得参数的数据长度
//
//
//*****************************************************************
unsigned short Parameter::GetParamDataLen()
{
	return m_paramDataLen;
}

//*****************************************************************
//取得参数的数据类型
//
//
//*****************************************************************
string Parameter::GetParamDataType()
{
	return m_paramDataType;
}

//*****************************************************************
//设置参数长度
//
//
//*****************************************************************
void Parameter::SetParamLen(unsigned short len)
{
	if(len <= MAX_PARAMETER_LEN)	m_paramDataLen = len;
	else m_paramDataLen = 0;
}

//*****************************************************************
//取得参数值,用来组帧
//
//
//*****************************************************************
bool Parameter::GetFrameData(void *des, int flag) //当flag = 0时,只返回参数值,当flag = 1时,返回参数号和参数值,当flag = 2 时,返回参数有效标志和参数值
{
    if((des != NULL)&&(m_paramVal != NULL)&&m_paramTime!=0xffffffff)
	{
		switch(flag)
		{
			case 0:memcpy(des, m_paramVal, m_paramDataLen);	
						 break;
			case 1:memcpy(des, &m_paramNo, 2);
						 memcpy((char*)des+2, m_paramVal, m_paramDataLen);
						 break;
            case 2:memcpy(des, 	&m_paramVaildFlag, 1);
						 memcpy((char*)des+1, m_paramVal, m_paramDataLen);
						 break;
			default:m_errorCode = USR_FLAG_NOT_MATCH;
							return false;
		}
	}
	else 
	{
		m_errorCode = USR_WRITE_NULL_POINTER;
		return false;
	}
	return true;
}

//*****************************************************************
//释放参数所占用的空间,准备容纳另一参数
//
//
//*****************************************************************
void Parameter::FreeParamSpace()
{
	m_tableNo = 0;															//复位表号成员变量
	m_paramNo = 0;															//复位参数号成员变量
	m_paramTitle = "";													//复位参数标题成员变量
	m_paramDataType = "";												//复位参数数据类型成员变量
	m_paramDataLen = 0;												  //复位参数数据长度成员变量
	m_nextParamNo = 0;
    m_paramTime = 0xffffffff;														//复位参数采集时间
	m_paramTitle.clear();
	if(NULL != m_paramVal) 
	{
		free(m_paramVal);   				//释放内存
		m_paramVal = NULL;					//置空指针
	}
}

//*****************************************************************
// 转换参数值为显示模式函数
// 将参数值转换为字符串，并返回
//  m_paramDataLen
//*****************************************************************
string Parameter::GetParamValDisp()
{
	unsigned char tempstr[9] = {0};
	char DispStr[17] = {0};
	int strLen = 0;
	
	if(NULL == m_paramVal)
	{
		return(DispStr);
	}
	//字符串类型   源码数据类型
    if((0 == m_paramDataType.compare("string"))||(0 == m_paramDataType.compare("code")))
	{
		if(m_paramDataLen > 8)
		{
			strLen = 8;
			memcpy(tempstr, m_paramVal, strLen);								//拷贝参数前八个字节，因为只需显示最大八个字节长
		}
		else
		{
			strLen = m_paramDataLen;
			memcpy(tempstr, m_paramVal, strLen);
		}
	}
	else 
	{
		strLen = m_paramDataLen;
		memcpy(tempstr, m_paramVal, strLen);
	}

	for(int i=0;i<strLen;i++)
	{
		DispStr[2*i] = GetHighChar(tempstr[i]);
		DispStr[2*i+1] = GetLowChar(tempstr[i]);
	}
	return(DispStr);
}

//*****************************************************************
// 转换字符值为十六进制数字符串函数
// 获取高位字符表示
// 
//*****************************************************************
char Parameter::GetHighChar(unsigned char c)
{
	char tempCharVal = '0';
	char tempVal = c/16;
	if(( tempVal >= 0 ) && (tempVal <= 9))
	{
		tempCharVal = tempCharVal + tempVal;
	}
	else if(( tempVal >= 10 ) && ( tempVal <= 15 ))
	{
		tempCharVal = 'A';
		tempCharVal = tempCharVal + ( tempVal - 10 );
	}
	return(tempCharVal);
}

//*****************************************************************
// 转换字符值为十六进制数字符串函数
// 获取低位字符表示
// 
//*****************************************************************
char Parameter::GetLowChar(unsigned char c)
{
	char tempCharVal = '0';
	char tempVal = c%16;
	if(( tempVal >= 0 ) && (tempVal <= 9))
	{
		tempCharVal = tempCharVal + tempVal;
	}
	else if(( tempVal >= 10 ) && ( tempVal <= 15 ))
	{
		tempCharVal = 'A';
		tempCharVal = tempCharVal + ( tempVal - 10 );
	}
	return(tempCharVal);
}

//*****************************************************************
//析构函数
// 
//
//*****************************************************************
Parameter::~Parameter()
{
	if(NULL != m_paramVal) 
	{
		free(m_paramVal);   				//释放内存
		m_paramVal = NULL;					//置空指针
	}
}













