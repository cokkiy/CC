#ifndef NUMERICALVALUERESULTSTRING_H
#define NUMERICALVALUERESULTSTRING_H
#include<stdio.h>
#include<string>

typedef unsigned int uint;

using namespace std;
class QNumericalValueResultString
{
public:
    QNumericalValueResultString();
    string getNumericalValueString(const double ,const string );
private:
    //H 十六进制（大写）
    string getHString(const double fParamValue);
    //h 十六进制（小写）
    string gethString(const double fParamValue);
    //b  二进制
    string getbString(const double fParamValue);
    //.xfunit
    string getDotxString(const double ,const string );
    //根据参数值的正负情况，选择显示“偏远”或“偏近”+参数值+“公里”
    //格式：? 大于0 : 小于0 : 等于0
    //格式：?偏远%.1f公里:偏近%.0f公里
    //格式：?偏远:偏近
    //格式：?偏远%.1f公里:偏近%.0f公里:正中靶心
    //格式：?偏远:偏近:正中靶心
    string getQuestionMarkString(const double ,const string );
    string getDefaultString(const double fParamValue);
};


#endif // NUMERICALVALUERESULTSTRING_H
