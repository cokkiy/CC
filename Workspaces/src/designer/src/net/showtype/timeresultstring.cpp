#include "timeresultstring.h"

QTimeResultString::QTimeResultString()
{

}

//时间转换
string QTimeResultString::GetTimeString(const double Value,const string type)
{
    if(!strcmp(type.c_str(), "T1"))
        return GetT1String(Value);
    else if(!strcmp(type.c_str(), "t1"))
        return Gett1String(Value);
    else if(!strcmp(type.c_str(), "T2"))
        return  GetT2String(Value);
    else if(!strcmp(type.c_str(), "t2"))
        return  Gett2String(Value);
    else if(!strcmp(type.c_str(), "T3"))
        return  GetT3String(Value);
    else if(!strcmp(type.c_str(), "t3"))
        return  Gett3String(Value);
    else if(!strcmp(type.c_str(), "T4"))
        return  GetT4String(Value);
    else if(!strcmp(type.c_str(), "t4"))
        return  Gett4String(Value);
    else if(!strcmp(type.c_str(), "t5"))
        return  Gett5String(Value);
    else if(!strcmp(type.c_str(), "t6"))
        return  Gett6String(Value);
    else
        return GettDotString(Value,type);
}

//T1显示格式
string QTimeResultString::GetT1String(const double fParamValue)
{
    unsigned int iTime = (unsigned int)(fParamValue + 5000);//0.1ms
    char str[128];
    ::sprintf(str,"%02d 时 %02d 分 %02d 秒",
                iTime/10000/3600,
                (iTime/10000%3600)/60,
                (iTime/10000%3600)%60);
    return str;
}

//t1显示格式
string QTimeResultString::Gett1String(const double fParamValue)
{
    unsigned int iTime = (unsigned int)(fParamValue + 5000);//0.1ms
    char str[128];
    ::sprintf(str,"%02d:%02d:%02d",
                iTime/10000/3600,
                (iTime/10000%3600)/60,
                (iTime/10000%3600)%60);
    return str;
}

//T2显示格式
string QTimeResultString::GetT2String(const double fParamValue)
{
    unsigned int iTime = (unsigned int)(fParamValue/10000);//0.1ms
    char str[128];
    sprintf(str,"%d秒",iTime);
    return str;
}
//t2显示格式
string QTimeResultString::Gett2String(const double fParamValue)
{
    unsigned int iTime = (unsigned int)(fParamValue/10000);//0.1ms
    char str[128];
    ::sprintf(str,"%ds",(int)iTime);
    return str;
}
//T3显示格式
//XX时XX分XX秒XXX毫秒
string QTimeResultString::GetT3String(const double fParamValue)
{
    char str[128];
    ::sprintf(str,"%02d 时 %02d 分 %02d 秒 %03d 毫秒",
              (int)(fParamValue/10000/3600),
              (int)(((int)(fParamValue/10000)%3600)/60),
              (int)(((int)(fParamValue/10000)%3600)%60),
              (int)((int)(fParamValue)%10000/10));
    return str;
}
//t3显示格式
//XX：XX：XX.XXX
string QTimeResultString::Gett3String(const double fParamValue)
{
    char str[128];
    ::sprintf(str,"%02d:%02d:%02d.%03d",
              (int)(fParamValue/10000/3600),
              (int)(((int)(fParamValue/10000)%3600)/60),
              (int)(((int)(fParamValue/10000)%3600)%60),
              (int)((int)(fParamValue)%10000/10));
    return str;
}
//T4显示格式
//xxxx.xxx秒
string QTimeResultString::GetT4String(const double fParamValue)
{
    double fTime = fParamValue/10000.0;
    char str[128];
    ::sprintf(str,"%.3f秒",fTime);
    return str;
}
//t4显示格式
//xxxx.xxxs
string QTimeResultString::Gett4String(const double fParamValue)
{
    double fTime = fParamValue/10000.0;
    char str[128];
    ::sprintf(str,"%.3fs",fTime);
    return str;
}
//t5显示格式
//XXXX.XXX
string QTimeResultString::Gett5String(const double fParamValue)
{
    double fTime = fParamValue/10000.0;
    char str[128];
    ::sprintf(str,"%.3f",fTime);
    return str;
}
//t6显示格式
string QTimeResultString::Gett6String(const double fParamValue)
{
    unsigned int iTime = (unsigned int)(fParamValue/10000);
    char str[128];
    ::sprintf(str,"%d",iTime);
    return str;
}

//t.xfunit
string QTimeResultString::GettDotString(const double fParamValue,const string atrriStr)
{
    char str[128];
    string formatStr = atrriStr;
    formatStr.replace(0, 2,"%.");
    ::sprintf(str,formatStr.data(),fParamValue/10000.0);
    return str;
}


