#include "dateresultstring.h"

QDateResultString::QDateResultString()
{
}

string QDateResultString::GetDateString(const double Value,const string type)
{
    if(!strcmp(type.c_str(), "LED_DT"))
        return getLEDDateString(Value);
    else
        return  getDayofYearString(Value);//D2000
}
//库尔勒LED日期 (XXXX年XX月XX日XX时XX分XX秒)
string QDateResultString::getLEDDateString(const double fParamValue)
{
    char str[128];

    return str;
}
//积日 2000年1月1日
string QDateResultString::getDayofYearString(const double fParamValue)
{
    char str[128];
    int year = 2000;
    int montn = 1;
    int day = 1;

    return str;
}
