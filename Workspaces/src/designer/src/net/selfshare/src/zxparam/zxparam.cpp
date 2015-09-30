// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：zxparam.cpp
// 文件摘要：指显系统参数类实现文件，用来实现指显参数类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#include "../datastruct.h"
#include "zxparam.h"
#include "C3I/CParamInfoRT.h"

//外部变量,C3I/PDXP包处理对象信息约定表
extern CParamInfoRT g_paramInfoRT;

//*****************************************************************
//构造函数
//
//
//*****************************************************************
ZXParam::ZXParam():Parameter()
{
	m_paramdate = 0;

	//是否超差
	m_bError = false;

	//是否为新值,对于源码、字符串类型参数该值始终为0
	//0表示首次，1表示旧值，2表示新值
	m_ucNew = 0;

	//原始值，不包括：原码和字符串
	memset(m_ucParamValue_Old, 0, 8);

	//备注
	m_pcParamNote = "";

	//数据下限
	m_dParamLorlmt = 0;

	//数据上限
	m_dParamUprlmt = 0;

	//参数的转换系数，量纲
	//用来对网络接收的数据进行处理
	m_dParamQuotiety = 0;

	//参数代号字符串
	m_pcParamID = "";

	//参数单位
	m_pcParamUnit = "";

	//参数转换后的存储类型
	m_ucChangeType = 0;

	//参数的表现类型 表现类型是指参数在指显终端以表格方式显示时
	//的显示形态。曲线绘制时不考虑此因素
	m_pcShowType = "";

	//参数传输类型
	m_ucTransType = 0;

	//理论值
	m_pcTheoryValue = "";

	//理论值范围
	m_pcValueRange = "";

	//索引
	m_ulGroupIndex = 0;
}

//*****************************************************************
//拷贝构造函数
//
//
//*****************************************************************
ZXParam::ZXParam(const ZXParam &a):Parameter(a)
{
	//拷贝子类成员变量
	m_paramdate = a.m_paramdate;
	m_bError = a.m_bError;
	m_ucNew = a.m_ucNew;

	memcpy(m_ucParamValue_Old, a.m_ucParamValue_Old, 8);

	//备注
	m_pcParamNote = a.m_pcParamNote;

	//数据下限
	m_dParamLorlmt = a.m_dParamLorlmt;

	//数据上限
	m_dParamUprlmt = a.m_dParamUprlmt;

	//参数的转换系数，量纲
	//用来对网络接收的数据进行处理
	m_dParamQuotiety = a.m_dParamQuotiety;

	//参数代号字符串
	m_pcParamID = a.m_pcParamID;

	//参数单位
	m_pcParamUnit = a.m_pcParamUnit;

	//参数转换后的存储类型
	m_ucChangeType = a.m_ucChangeType;

	//参数的表现类型 表现类型是指参数在指显终端以表格方式显示时
	//的显示形态。曲线绘制时不考虑此因素
	m_pcShowType = a.m_pcShowType;

	//参数传输类型
	m_ucTransType = a.m_ucTransType;

	//理论值
	m_pcTheoryValue = a.m_pcTheoryValue;

	//理论值范围
	m_pcValueRange = a.m_pcValueRange;

	//索引
	m_ulGroupIndex = a.m_ulGroupIndex;
}

//*****************************************************************
//赋值构造函数
//
//
//*****************************************************************
ZXParam & ZXParam::operator =(const ZXParam &a)
{
	Parameter::operator =(a);

	//拷贝子类成员变量
	m_paramdate = a.m_paramdate;
	m_bError = a.m_bError;
	m_ucNew = a.m_ucNew;

	memcpy(m_ucParamValue_Old, a.m_ucParamValue_Old, 8);

	//备注
	m_pcParamNote = a.m_pcParamNote;

	//数据下限
	m_dParamLorlmt = a.m_dParamLorlmt;

	//数据上限
	m_dParamUprlmt = a.m_dParamUprlmt;

	//参数的转换系数，量纲
	//用来对网络接收的数据进行处理
	m_dParamQuotiety = a.m_dParamQuotiety;

	//参数代号字符串
	m_pcParamID = a.m_pcParamID;

	//参数单位
	m_pcParamUnit = a.m_pcParamUnit;

	//参数转换后的存储类型
	m_ucChangeType = a.m_ucChangeType;

	//参数的表现类型 表现类型是指参数在指显终端以表格方式显示时
	//的显示形态。曲线绘制时不考虑此因素
	m_pcShowType = a.m_pcShowType;

	//参数传输类型
	m_ucTransType = a.m_ucTransType;

	//理论值
	m_pcTheoryValue = a.m_pcTheoryValue;

	//理论值范围
	m_pcValueRange = a.m_pcValueRange;

	//索引
	m_ulGroupIndex = a.m_ulGroupIndex;

	return *this;
}

//析构函数
ZXParam::~ZXParam()
{

}

//*****************************************************************
//根据表号和参数号，
//获取从公用信息约定表获取参数相关信息，填入参数类的各个成员变量
//返回值为false，原因可能为配置有误，表号不存在，参数号不存在，内存不足
//关于paramno的取值,当paramno为0时，表示是完整信息帧的第一个参数，paramno不为0时，其取值为参数号
//*****************************************************************
bool ZXParam::GetZXParamInfo(unsigned short tableno, unsigned short paramno)
{
    //获取信息约定表
    TableMap* tableMap = g_paramInfoRT.getTableMap();
    m_tableNo = tableno;
    if (0 != paramno)
    {
        m_paramNo = paramno;
        auto table = tableMap->find(tableno);
        if (table != tableMap->end()) //找到表号
        {
            //在信息约定表中查找表号对应项
            auto param = table->second->find(paramno);
            if (param != table->second->end())
            {
                //信息约定表中存在指定的表号和参数号
                stru_Param* para = param->second;
                m_paramTitle = para->pcParamName.toStdString();
                m_pcParamID = para->pcParamID.toStdString();
                m_ucTransType = para->ucTransType;
                m_ucChangeType = para->ucChangeType;
                m_paramDataType = para->ucTransType;
                m_paramDataLen = para->usParamLen;
                m_dParamLorlmt = para->dParamLorlmt;
                m_dParamUprlmt = para->dParamUprlmt;
                m_dParamQuotiety = para->dParamQuotiety;
                m_pcParamUnit = para->pcParamUnit.toStdString();
                m_pcShowType = para->pcShowType.toStdString();
                m_pcTheoryValue = para->pcTheoryValue.toStdString();
                m_pcValueRange = para->pcValueRange.toStdString();
                m_pcParamNote = para->pcParamNote.toStdString();
                m_ulGroupIndex = para->ulGroupIndex;
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
                m_pcParamID = para->pcParamID.toStdString();
                m_ucTransType = para->ucTransType;
                m_ucChangeType = para->ucChangeType;
                m_paramDataType = para->ucTransType;
                m_paramDataLen = para->usParamLen;
                m_dParamLorlmt = para->dParamLorlmt;
                m_dParamUprlmt = para->dParamUprlmt;
                m_dParamQuotiety = para->dParamQuotiety;
                m_pcParamUnit = para->pcParamUnit.toStdString();
                m_pcShowType = para->pcShowType.toStdString();
                m_pcTheoryValue = para->pcTheoryValue.toStdString();
                m_pcValueRange = para->pcValueRange.toStdString();
                m_pcParamNote = para->pcParamNote.toStdString();
                m_ulGroupIndex = para->ulGroupIndex;
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

unsigned char ZXParam::GetParamTranType()
{
    return m_ucTransType;
}

unsigned char ZXParam::GetParamConvType()
{
    return m_ucChangeType;
}

//设置参数日期
void ZXParam::SetZXParamDate(unsigned int& ddate)
{
	m_paramdate = ddate;
}

//获取参数日期
unsigned int ZXParam::GetZXParamDate()
{
	return m_paramdate;
}

//设置参数超差
void ZXParam::SetZXParamError(bool& error)
{
    m_bError = error;
}

//设置参数是否为新值
void ZXParam::SetZXParamNew(unsigned char n)
{
	m_ucNew = n;
}

//获取参数是否为新值标识
unsigned int ZXParam::GetZXParamNew()
{
    return m_ucNew;
}

//设置参数原始值
void ZXParam::SetZXParamOldValue(const void* pValue)
{
	memcpy(m_ucParamValue_Old, (unsigned char*)pValue, 8);
}

//获取参数原始值
void ZXParam::GetZXParamOldValue(unsigned char* pValue)
{
	memcpy(pValue, m_ucParamValue_Old, 8);

	m_ucNew = 1;
}

//设置参数备注
void ZXParam::SetZXParamNote(const string& note)
{
	m_pcParamNote = note;
}

//获取参数备注
string ZXParam::GetZXParamNote()
{
	return m_pcParamNote;
}

//设置参数下限
void ZXParam::SetZXParamLorlmt(const double& ll)
{
	m_dParamLorlmt = ll;
}

//获取参数下限
double ZXParam::GetZXParamLorlmt()
{
	return m_dParamLorlmt;
}

//设置参数上限
void ZXParam::SetZXParamUprlmt(const double& ul)
{
	m_dParamUprlmt = ul;
}

//获取参数上限
double ZXParam::GetZXParamUprlmt()
{
	return m_dParamUprlmt;
}

//设置参数量纲
void ZXParam::SetZXParamQuotiety(const double& qt)
{
	m_dParamQuotiety = qt;
}

//获取参数量纲
double ZXParam::GetZXParamQuotiety()
{
	return m_dParamQuotiety;
}

//设置参数代号
void ZXParam::SetZXParamCode(const string& code)
{
	m_pcParamID = code;
}

//获取参数代号
string ZXParam::GetZXParamCode()
{
	return m_pcParamID;
}

//设置参数单位
void ZXParam::SetZXParamUnit(const string& unit)
{
	m_pcParamUnit = unit;
}

//获取参数单位
string ZXParam::GetZXParamUnit()
{
	return m_pcParamUnit;
}

//设置参数显示类型
void ZXParam::SetZXParamShowType(const string& dt)
{
	m_pcShowType = dt;
}

//获取参数显示类型
string ZXParam::GetZXParamShowType()
{
	return m_pcShowType;
}

//设置参数理论值
void ZXParam::SetZXParamTheoryValue(const string& tv)
{
	m_pcTheoryValue = tv;
}

//获取参数理论值
string ZXParam::GetZXParamTheoryValue()
{
	return m_pcTheoryValue;
}


//设置参数理论值范围
void ZXParam::SetZXParamTheoryRange(const string& vr)
{
	m_pcValueRange = vr;
}

//获取参数理论值范围
string ZXParam::GetZXParamTheoryRange()
{
	return m_pcValueRange;
}

//设置参数索引
void ZXParam::SetZXParamGroupIndex(unsigned long& gi)
{
	m_ulGroupIndex = gi;
}

//获取参数索引
unsigned long ZXParam::GetZXParamGroupIndex()
{
	return m_ulGroupIndex;
}


