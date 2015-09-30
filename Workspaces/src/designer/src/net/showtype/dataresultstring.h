#ifndef DATARESULTSTRING_H
#define DATARESULTSTRING_H
#include "../../net/net_global.h"
#include "../selfshare/src/datastruct.h"
#include<string>
using namespace std;
class QDataResultString
{
public:
    QDataResultString(const double ,const string ,unsigned char ,const string ,const string );
    string getResultString();
    void analysisShowAttrString();
private:
    string m_paramShowAttrString;//参数显示属性字符串(图元配置的属性)

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
