#ifndef ABSTRACTPARAM_H
#define ABSTRACTPARAM_H
#include "net_global.h"
#include <qvariant.h>
#include "selfshare/src/zxparam/zxparam.h"
class NETSHARED_EXPORT AbstractParam : public ZXParam
{
public:
    AbstractParam();
    ~AbstractParam();
    bool canCompute();
    bool getValueFromCode(double&);
    bool getValue(QVariant&);
    bool setValue(double val);
    bool getValue(double& val);
    // 获取参数的小写的源码
    bool getCode(std::string & result);
    AbstractParam & operator =(const AbstractParam &aParam)
    {
        m_tableNo = aParam.m_tableNo;  				 //表号
        m_paramNo = aParam.m_paramNo;          //参数号
        m_paramTitle = aParam.m_paramTitle;
        m_paramDataType = aParam.m_paramDataType;    //参数数据类型
        m_paramDataLen = aParam.m_paramDataLen; //参数数据长度
        m_ucNew = aParam.m_ucNew; //是否为新值,对于源码、字符串类型参数该值始终为0 0表示首次，1表示旧值，2表示新值
        m_paramTime = aParam.m_paramTime;//参数采集时间,绝对时
        m_paramdate = aParam.m_paramdate; //参数采集日期,绝对时
        m_bError = aParam.m_bError;
        m_dParamLorlmt = aParam.m_dParamLorlmt;
        m_dParamUprlmt = aParam.m_dParamUprlmt;
        m_dParamQuotiety = aParam.m_dParamQuotiety;
        m_pcParamID = aParam.m_pcParamID;
        m_pcParamUnit = aParam.m_pcParamUnit;
        m_ucChangeType = aParam.m_ucChangeType;
        m_pcShowType = aParam.m_pcShowType;
        m_pcTheoryValue = aParam.m_pcTheoryValue;
        m_pcValueRange = aParam.m_pcValueRange;
        m_ulGroupIndex = aParam.m_ulGroupIndex;
        m_ucTransType = aParam.m_ucTransType;
        m_pcParamNote = aParam.m_pcParamNote;
        m_val = aParam.m_val;
        Parameter::operator =(aParam);
        return *this;
    }

private:
    //参数存放的浮点型的数据
    double m_val;
public:
    //设置参数表号
    void SetZXParamTableNo(const unsigned short& bh);
    //获取参数表号
    unsigned short GetZXParamTableNo();

    //设置参数号
    void SetZXParamNo(const unsigned short& bh);
    //获取参数号
    unsigned short GetZXParamNo();

    //设置参数代号
    void SetZXParamCode(const string& code);
    //获取参数代号
    string GetZXParamCode();

    //设置参数名称
    void SetZXParamTitle(const string& title);
    //获取参数名称
    string GetZXParamTitle();

    //设置参数下限
    void SetZXParamLorlmt(const double& ll);
    //获取参数下限
    double GetZXParamLorlmt();

    //设置参数上限
    void SetZXParamUprlmt(const double& ul);
    //获取参数上限
    double GetZXParamUprlmt();

    //设置参数量纲
    void SetZXParamQuotiety(const double& qt);
    //获取参数量纲
    double GetZXParamQuotiety();

    //设置参数类型
    void SetZXParamDataType(const string& type);
    //获取参数类型
    string GetZXParamDataType();

    //设置参数长度
    void SetZXParamDataLength(const unsigned short& len);
    //获取参数长度
    unsigned short GetZXParamDataLength();

    //设置参数单位
    void SetZXParamUnit(const string& unit);
    //获取参数单位
    string GetZXParamUnit();

    //设置参数显示类型
    void SetZXParamShowType(const unsigned char& dt);
    //获取参数显示类型
    string GetZXParamShowType();

    //设置参数传输类型
    void SetZXParamTranType(const unsigned char& vr);
    //获取参数传输类型
    unsigned char GetZXParamTranType();

    //设置参数理论值
    void SetZXParamTheoryValue(const string& tv);
    //获取参数理论值
    string GetZXParamTheoryValue();

    //设置参数显示范围
    void SetZXParamTheoryRange(const string& vr);
    //获取参数显示范围
    string GetZXParamTheoryRange();

    //设置参数索引
    void SetZXParamGroupIndex(unsigned long& gi);
    //获取参数索引
    unsigned long GetZXParamGroupIndex();

    //设置参数备注
    void SetZXParamNote(const string& note);
    //获取参数备注
    string GetZXParamNote();

/*private:
    unsigned short m_tableNo;  		//表号
    unsigned short m_paramNo;       //参数号
    string m_paramID;    //参数代号
    string m_paramTitle;    //参数名称
    unsigned char m_paramDataType; //参数数据类型
    unsigned short m_paramDataLen;     	//参数数据长度
    double m_paramLorlmt;    //数据下限
    double m_paramUprlmt;    //数据上限
    double m_paramQuotiety;    //参数的转换系数，量纲
    string m_paramUnit;    //参数单位
    //参数的表现类型 表现类型是指参数在指显终端以表格方式显示时
    //的显示形态。曲线绘制时不考虑此因素
    string m_showType;
    unsigned char m_transType;    //参数传输类型
    string m_theoryValue;    //理论值
    string m_valueRange;    //理论值范围
    unsigned long m_groupIndex;    //索引
    string m_paramNote;    //备注*/
};

#endif // ABSTRACTPARAM_H
