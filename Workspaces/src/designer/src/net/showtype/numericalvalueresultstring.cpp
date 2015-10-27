#include "numericalvalueresultstring.h"
#include <math.h>
#include <string.h>
#if defined(_WIN32)
#define  uint unsigned int
#endif
QNumericalValueResultString::QNumericalValueResultString()
{
}
//H 十六进制（大写）
string QNumericalValueResultString::getHString(const double fParamValue)
{
    char str[256];
    sprintf(str,"%08XH",(unsigned int)fParamValue);
    return str;
}

//h 十六进制（小写）
string QNumericalValueResultString::gethString(const double fParamValue)
{
    char str[256];
    sprintf(str,"%08xh",(unsigned int)fParamValue);
    return str;
}
//b  二进制
string QNumericalValueResultString::getbString(const double fParamValue)
{
    string str;
    uint iValue = (uint)fParamValue;
    int i = 0;
    int iBIN[1024];

    while(iValue)
    {
        iBIN[i] = iValue%2;
        iValue = iValue/2;
        i++;
    }
    for(int j = i-1;j >= 0;j--)
    {
        char tempstr[2];
        sprintf(tempstr,"%d",iBIN[j]);
        str += tempstr;
    }
    str.append("b");
    return str;
}

//.xfunit
string QNumericalValueResultString::getDotxString(const double fParamValue,const string showAtrriStr)
{
    char str[128];
    string format = "%";
    format = format + showAtrriStr;
    sprintf(str,format.data(),fParamValue);
    return str;
}

string QNumericalValueResultString::getQuestionMarkString(const double fParamValue,const string showAtrriStr)
{
        int minusPos = showAtrriStr.find(":",0);
        int equalPos = showAtrriStr.find(":",minusPos + 1);
        string formatStr = "";
        char str[256];
        //配置了“等于0”的选项
        if (equalPos != showAtrriStr.npos)
        {
            //值等于0
            if (fabs(fParamValue)<0.0000000001)
            {
                formatStr = showAtrriStr.substr(equalPos+1,showAtrriStr.length());
                sprintf(str,"%s",formatStr.data());
                return str;
            }
        }
        if (fParamValue >= 0)
        {
            formatStr = showAtrriStr.substr(1,minusPos-1);
        }
        else if (fParamValue < 0)
        {
            formatStr = showAtrriStr.substr(minusPos+1,equalPos-minusPos-1);
        }
        //如果没有配置“%”号，则添加“%.3f”。
        if (showAtrriStr.find('%',0) == showAtrriStr.npos)
        {
            formatStr += "%.3f";
        }

        sprintf(str,formatStr.data(),fabs(fParamValue));
        return str;
}

string  QNumericalValueResultString::getDefaultString(const double fParamValue)
{
    char* str = 0;
    sprintf(str,"%.3f",fParamValue);
    return str;
}

string QNumericalValueResultString::getNumericalValueString(const double fParamValue,const string showAtrriStr)
{

    if(!strcmp(showAtrriStr.c_str(), "h"))
        return gethString(fParamValue);
    else if(!strcmp(showAtrriStr.c_str(), "H"))
        return getHString(fParamValue);
    else if(!strcmp(showAtrriStr.c_str(), "b"))
        return getbString(fParamValue);
    else if(showAtrriStr.find(".",0) == 0)
        return getDotxString(fParamValue,showAtrriStr);
    else if(showAtrriStr.find("?",0) == 0)
        return getQuestionMarkString(fParamValue,showAtrriStr);
    else
        return getDefaultString(fParamValue);
}


