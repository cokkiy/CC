#include "abstractparam.h"

AbstractParam::AbstractParam():ZXParam()
{
    m_val = 0.0;
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
        char temp[MAX_PARAMETER_VAL_LEN] = {0};
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
            data = tempVal*GetZXParamQuotiety();
        else
            return false;
    }
        break;
    case tp_BYTE:
    {
        unsigned char tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetZXParamQuotiety();
        else
            return false;
    }
        break;
    case tp_short:
    {
        short tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetZXParamQuotiety();
        else
            return false;
    }
        break;
    case tp_WORD:
    {
        unsigned short tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetZXParamQuotiety();
        else
            return false;
    }
        break;
    case tp_long:
    {
        int tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetZXParamQuotiety();
        else
            return false;
    }
        break;
    case tp_DWORD:
    case tp_Date:
    {
        unsigned int tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetZXParamQuotiety();
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
            data = tempVal*GetZXParamQuotiety();
        else
            return false;
    }
        break;
    case tp_double:
    {
        if(GetFrameData(&data))
            data = data*GetZXParamQuotiety();
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
            data = tempVal*GetZXParamQuotiety();
        else
            return false;
    }
        break;
    case tp_BYTE:
    {
        unsigned char tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetZXParamQuotiety();
        else
            return false;
    }
        break;
    case tp_short:
    {
        short tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetZXParamQuotiety();
        else
            return false;
    }
        break;
    case tp_WORD:
    {
        unsigned short tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetZXParamQuotiety();
        else
            return false;
    }
        break;
    case tp_long:
    {
        int tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetZXParamQuotiety();
        else
            return false;
    }
        break;
    case tp_DWORD:
    case tp_Date:
    {
        unsigned int tempVal = 0;
        if(GetFrameData(&tempVal))
            data = tempVal*GetZXParamQuotiety();
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
            data = tempVal*GetZXParamQuotiety();
        else
            return false;
    }
        break;
    case tp_double:
    {
        if(GetFrameData(&data))
            data = data*GetZXParamQuotiety();
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
    m_val = data;
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
    if(GetFrameData(ba.data()))
    {
        result = std::string(ba.toHex().constData());
        return true;
    }
    else
        return false;
}

//设置参数表号
void AbstractParam::SetZXParamTableNo(const unsigned short& bh)
{
    m_paramNo = bh;
}
//获取参数表号
unsigned short AbstractParam::GetZXParamTableNo()
{
    return m_tableNo;
}

//设置参数号
void AbstractParam::SetZXParamNo(const unsigned short& bh)
{
    m_paramNo = bh;
}
//获取参数号
unsigned short AbstractParam::GetZXParamNo()
{
    return m_paramNo;
}

//设置参数代号
void AbstractParam::SetZXParamCode(const string& code)
{
    m_pcParamID = code;
}
//获取参数代号
string AbstractParam::GetZXParamCode()
{
    return m_pcParamID;
}

//设置参数名称
void AbstractParam::SetZXParamTitle(const string& title)
{
    m_paramTitle = title;
}
//获取参数名称
string AbstractParam::GetZXParamTitle()
{
    return m_paramTitle;
}

//设置参数下限
void AbstractParam::SetZXParamLorlmt(const double& ll)
{
    m_dParamLorlmt = ll;
}
//获取参数下限
double AbstractParam::GetZXParamLorlmt()
{
    return m_dParamLorlmt;
}

//设置参数上限
void AbstractParam::SetZXParamUprlmt(const double& ul)
{
    m_dParamUprlmt = ul;
}
//获取参数上限
double AbstractParam::GetZXParamUprlmt()
{
    return m_dParamUprlmt;
}

//设置参数量纲
void AbstractParam::SetZXParamQuotiety(const double& qt)
{
    m_dParamQuotiety = qt;
}
//获取参数量纲
double AbstractParam::GetZXParamQuotiety()
{
    return m_dParamQuotiety;
}

//设置参数类型
void AbstractParam::SetZXParamDataType(const string& type)
{
    m_paramDataType = type;
}
//获取参数类型
string AbstractParam::GetZXParamDataType()
{
    return m_paramDataType;
}

//设置参数长度
void AbstractParam::SetZXParamDataLength(const unsigned short& len)
{
    m_paramDataLen = len;
}
//获取参数长度
unsigned short AbstractParam::GetZXParamDataLength()
{
    return m_paramDataLen;
}

//设置参数单位
void AbstractParam::SetZXParamUnit(const string& unit)
{
    m_pcParamUnit = unit;
}
//获取参数单位
string AbstractParam::GetZXParamUnit()
{
    return m_pcParamUnit;
}

//设置参数显示类型
void AbstractParam::SetZXParamShowType(const unsigned char& dt)
{
    m_ucTransType = dt;
}
//获取参数显示类型
string AbstractParam::GetZXParamShowType()
{
    return m_pcShowType;
}
//设置参数传输类型
void AbstractParam::SetZXParamTranType(const unsigned char& vr)
{
    m_ucTransType = vr;
}
//获取参数传输类型
unsigned char AbstractParam::GetZXParamTranType()
{
    return m_ucTransType;
}

//设置参数理论值
void AbstractParam::SetZXParamTheoryValue(const string& tv)
{
    m_pcTheoryValue = tv;
}
//获取参数理论值
string AbstractParam::GetZXParamTheoryValue()
{
   return m_pcTheoryValue;
}

//设置参数显示范围
void AbstractParam::SetZXParamTheoryRange(const string& vr)
{
    m_pcValueRange = vr;
}
//获取参数显示范围
string AbstractParam::GetZXParamTheoryRange()
{
    return m_pcValueRange;
}
//设置参数索引
void AbstractParam::SetZXParamGroupIndex(unsigned long& gi)
{
    m_ulGroupIndex = gi;
}
//获取参数索引
unsigned long AbstractParam::GetZXParamGroupIndex()
{
    return m_ulGroupIndex;
}
//设置参数备注
void AbstractParam::SetZXParamNote(const string& note)
{
    m_pcParamNote = note;
}
//获取参数备注
string AbstractParam::GetZXParamNote()
{
    return m_pcParamNote;
}
