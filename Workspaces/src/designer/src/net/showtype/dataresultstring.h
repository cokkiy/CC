#ifndef DATARESULTSTRING_H
#define DATARESULTSTRING_H
#include "../../net/net_global.h"
#include "../selfshare/src/datastruct.h"
#include "LFormula.h"
#include<string>

class QDataResultString
{
public:
    enum DisplayType{
        Value,
        Time,
        Date,
        Code,
        Other
    };
    /*!
      构造函数
      @param LFormula* formula  LFormula的指针
      @param string dataStr  参数数据的字符串
      @param QVariant& result  显示格式的字符串
    */
    QDataResultString(LFormula* formula,const string dataStr,const string dispStr);
    string getResultString();
    void analysisShowAttrString();
private:
    string m_computeStr;//参数数据字符串(图元配置的属性)
    string m_paramShowAttrString;//参数显示属性字符串(图元配置的属性)
    LFormula* m_formula;//公式解释器的指针

    string m_ShowAttrStringHeader;
    string m_ShowAttrString;
    string m_ShowAttrStringTail;
    unsigned char m_ShowAttrType;

    double m_paramValue;//参数值
    string resultString;//结果字符串
    string m_paramUnit;//参数单位(信息约定表设置的属性)

    unsigned char m_paramDataType; //参数类型(信息约定表设置的属性)
    string m_paramShowType; //参数显示类型(信息约定表设置的属性)

    string GetDefaultString(const double);
};

#endif // DATARESULTSTRING_H
