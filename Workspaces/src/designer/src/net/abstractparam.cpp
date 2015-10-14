#include "abstractparam.h"
#include <QDebug>
//参数值最大长度（主要指字符串和原码类型）
const unsigned short MAX_PARAM_VAL_LEN = 4*1024;
AbstractParam::AbstractParam()
{
    m_val = 0.0;
    m_transType = 0xff;
    m_changeType = 0xff;
    m_paramDate = 0xffffffff;
    m_paramTime = 0xffffffff;
    m_paramVal = NULL;
    m_paramQuotiety = 1.0;
    m_ucNew = First;
}
//析构函数
AbstractParam::~AbstractParam()
{

}
bool AbstractParam::canCompute()
{
    switch(GetParamTranType())
    {
    case tp_char:
    case tp_BYTE:
    case tp_short:
    case tp_WORD:
    case tp_long:
    case tp_DWORD:
    case tp_PMTime:
    case tp_Date:
    case tp_float:
    case tp_double:
    {
        return true;
    }
        break;
    case tp_String:
    case tp_Code:
    default:
    {
        return false;
    }
        break;
    }
}
bool AbstractParam::getValue(QVariant&result)
{
    double data = 0;
    switch(GetParamTranType())
    {
    case tp_String:
    {
        char temp[MAX_PARAM_VAL_LEN] = {0};
        if(GetFrameData(temp))
        {
            result = QVariant(temp);
            return true;
        }
        else
            return false;
    }
        break;
    case tp_Code:
    {
        QByteArray ba;
        ba.resize(GetParamDataLen());
        if(GetFrameData(ba.data()))
        {
            result = QVariant(ba);
            return true;
        }
        else
            return false;
    }
        break;
    case tp_char:
    {
        char tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_BYTE:
    {
        unsigned char tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_short:
    {
        short tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_WORD:
    {
        unsigned short tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_long:
    {
        int tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_DWORD:
    case tp_Date:
    {
        unsigned int tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    // PMTime的数据格式统一为0.1毫秒的整数，不进行量纲运算,andrew,20150929
    case tp_PMTime:
    {
        unsigned int tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal/**GetZXParamQuotiety()*/;
        else
            return false;
    }
        break;
    case tp_float:
    {
        float tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_double:
    {
        if(GetFrameData(&data))
            data = data*GetParamQuotiety();
        else
            return false;
    }
        break;
    default:
        return false;
        break;
    }
    result = QVariant(data);
    return true;
}

bool AbstractParam::getValueFromCode(double& data)
{
    switch(GetParamTranType())
    {
    case tp_char:
    {
        char tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_BYTE:
    {
        unsigned char tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_short:
    {
        short tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_WORD:
    {
        unsigned short tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_long:
    {
        int tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_DWORD:
    case tp_Date:
    {
        unsigned int tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    // PMTime的数据格式统一为0.1毫秒的整数，不进行量纲运算,andrew,20150929
    case tp_PMTime:
    {
        unsigned int tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal/**GetZXParamQuotiety()*/;
        else
            return false;
    }
        break;
    case tp_float:
    {
        float tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_double:
    {
        if(GetFrameData(&data))
            data = data*GetParamQuotiety();
        else
            return false;
    }
        break;
    case tp_String:
    case tp_Code:
    default:
    {
        return false;
    }
        break;
    }
    //为减少计算，提高效率，将参数的数据临时存放到m_val上，便于读取,andrew,20150929
    setValue(data);
    return true;
}
bool AbstractParam::setValue(double val)
{
    m_val = val;
    return true;
}
bool AbstractParam::getValue(double& val)
{
    if(GetParamTranType() == tp_String
            || GetParamTranType() == tp_Code
            || GetParamTime() == 0xffffffff)
    {
        return false;
    }
    val = m_val;
    return true;
}

bool AbstractParam::getCode(std::string & result)
{
    QByteArray ba;
    ba.resize(GetParamDataLen());
    void* data = ba.data();
    if(GetFrameData(data))
    {
        result = std::string(ba.toHex().constData());
        return true;
    }
    else
        return false;
}
AbstractParam & AbstractParam::operator =(const AbstractParam &aParam)
{
    m_ucNew = aParam.m_ucNew;
    m_tableNo = aParam.m_tableNo;
    m_paramNo = aParam.m_paramNo;
    m_paramName = aParam.m_paramName;
    m_paramDataType = aParam.m_paramDataType;
    m_paramDataLen = aParam.m_paramDataLen;
    m_paramTime = aParam.m_paramTime;
    m_paramDate = aParam.m_paramDate;
    m_paramLorlmt = aParam.m_paramLorlmt;
    m_paramUprlmt = aParam.m_paramUprlmt;
    m_paramQuotiety = aParam.m_paramQuotiety;
    m_paramCode = aParam.m_paramCode;
    m_paramUnit = aParam.m_paramUnit;
    m_changeType = aParam.m_changeType;
    m_dispType = aParam.m_dispType;
    m_theoryValue = aParam.m_theoryValue;
    m_valueRange = aParam.m_valueRange;
    m_ulGroupIndex = aParam.m_ulGroupIndex;
    m_transType = aParam.m_transType;
    m_pcParamNote = aParam.m_pcParamNote;
    m_val = aParam.m_val;
    //释放本对象的参数网络传输值指针的空间
    if(NULL != m_paramVal)
    {
        free(m_paramVal);
        m_paramVal = NULL;
    }
    //如果被赋值对象的参数网络传输值指针不为空
    if(NULL != aParam.m_paramVal)
    {
        //给本对象的变量指针分配空间
        m_paramVal = malloc(m_paramDataLen);
        //如果内存不足
        if(NULL == m_paramVal)
        {
            return *this;
        }
        //拷贝参数网络传输值
        memcpy(m_paramVal, aParam.m_paramVal, m_paramDataLen);
    }
    return *this;
}
//设置参数表号
void AbstractParam::SetTableNo(const unsigned short& bh)
{
    m_tableNo = bh;
}
//获取参数表号
unsigned short AbstractParam::GetTableNo()
{
    return m_tableNo;
}

//设置参数号
void AbstractParam::SetParamNo(const unsigned short& bh)
{
    m_paramNo = bh;
}
//获取参数号
unsigned short AbstractParam::GetParamNo()
{
    return m_paramNo;
}

//设置参数代号
void AbstractParam::SetParamCode(const string& code)
{
    m_paramCode = code;
}
//获取参数代号
string AbstractParam::GetParamCode()
{
    return m_paramCode;
}

//设置参数名称
void AbstractParam::SetParamName(const string& title)
{
    m_paramName = title;
}
//获取参数名称
string AbstractParam::GetParamName()
{
    return m_paramName;
}

//设置参数下限
void AbstractParam::SetParamLorlmt(const double& ll)
{
    m_paramLorlmt = ll;
}
//获取参数下限
double AbstractParam::GetParamLorlmt()
{
    return m_paramLorlmt;
}

//设置参数上限
void AbstractParam::SetParamUprlmt(const double& ul)
{
    m_paramUprlmt = ul;
}
//获取参数上限
double AbstractParam::GetParamUprlmt()
{
    return m_paramUprlmt;
}

//设置参数量纲
void AbstractParam::SetParamQuotiety(const double& qt)
{
    m_paramQuotiety = qt;
}
//获取参数量纲
double AbstractParam::GetParamQuotiety()
{
    return m_paramQuotiety;
}

void AbstractParam::SetParamConvType(unsigned char type)
{
    m_changeType = type;
}

unsigned char AbstractParam::GetParamConvType()
{
    return m_changeType;
}
//设置参数长度
void AbstractParam::SetParamDataLen(const unsigned short& len)
{
    m_paramDataLen = len;
}
//获取参数长度
unsigned short AbstractParam::GetParamDataLen()
{
    return m_paramDataLen;
}

//设置参数单位
void AbstractParam::SetParamUnit(const string& unit)
{
    m_paramUnit = unit;
}
//获取参数单位
string AbstractParam::GetParamUnit()
{
    return m_paramUnit;
}

//设置参数显示类型
void AbstractParam::SetParamShowType(const string& showtype)
{
    m_dispType = showtype;
}
//获取参数显示类型
string AbstractParam::GetParamShowType()
{
    return m_dispType;
}
//设置参数传输类型
void AbstractParam::SetParamTranType(const unsigned char& vr)
{
    m_transType = vr;
}
//获取参数传输类型
unsigned char AbstractParam::GetParamTranType()
{
    return m_transType;
}

//设置参数理论值
void AbstractParam::SetParamTheoryValue(const string& tv)
{
    m_theoryValue = tv;
}
//获取参数理论值
string AbstractParam::GetParamTheoryValue()
{
   return m_theoryValue;
}

//设置参数显示范围
void AbstractParam::SetParamTheoryRange(const string& vr)
{
    m_valueRange = vr;
}
//获取参数显示范围
string AbstractParam::GetParamTheoryRange()
{
    return m_valueRange;
}
//设置参数索引
void AbstractParam::SetParamGroupIndex(unsigned int gi)
{
    m_ulGroupIndex = gi;
}
//获取参数索引
unsigned int AbstractParam::GetParamGroupIndex()
{
    return m_ulGroupIndex;
}
//设置参数备注
void AbstractParam::SetParamNote(const string& note)
{
    m_pcParamNote = note;
}
//获取参数备注
string AbstractParam::GetParamNote()
{
    return m_pcParamNote;
}
void AbstractParam::SetParamStatus(unsigned char ucNew)
{
    m_ucNew = ucNew;
}

unsigned int AbstractParam::GetParamStatus()
{
    return m_ucNew;
}

void AbstractParam::SetParamDate(unsigned int date)
{
    m_paramDate = date;
}

unsigned int AbstractParam::GetParamDate()
{
    return m_paramDate;
}
void AbstractParam::SetParamTime(unsigned int time)
{
    m_paramTime = time;
}

unsigned int AbstractParam::GetParamTime()
{
    return m_paramTime;
}

//*****************************************************************
//取得参数值,用来组帧
//
//
//*****************************************************************
bool AbstractParam::GetFrameData(void *des)
{
    if((des != NULL)&&(m_paramVal != NULL)&&m_paramTime!=0xffffffff)
    {
        memcpy(des, m_paramVal, m_paramDataLen);
        return true;
    }
    return false;
}

//*****************************************************************
//释放参数所占用的空间,准备容纳另一参数
//
//
//*****************************************************************
void AbstractParam::FreeParamSpace()
{
    if(NULL != m_paramVal)
    {
        free(m_paramVal);   				//释放内存
        m_paramVal = NULL;					//置空指针
    }
}

void AbstractParam::InitParamSpace()
{
    switch(m_transType)
    {
    case tp_String:
    case tp_Code:
    {
        m_paramVal = malloc(MAX_PARAM_VAL_LEN);
        break;
    }
    default:
    {
        m_paramVal = malloc(m_paramDataLen);        
        break;
    }
    }
    if(m_paramVal==0)
        qCritical() << "The Param Space is Enough,No Space!";
}
void* AbstractParam::GetParamValuePoint()
{
    return m_paramVal;
}
string AbstractParam::GetTypeString(unsigned char type)
{
    if(type<DataTypeNum)
        return DataTypeStrings[type];
    return "UnKown";
}
