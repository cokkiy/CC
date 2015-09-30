#ifndef TIMERESULTSTRING_H
#define TIMERESULTSTRING_H

#include "dataresultstring.h"
#include<stdlib.h>
#include<stdio.h>
#include<string>

using namespace std;
class QTimeResultString
{
public:
    QTimeResultString();
    //时间转换
    string GetTimeString(const double Value,const string type);
private:
    //T1显示格式
    string GetT1String(const double fParamValue);

    //t1显示格式
    string Gett1String(const double fParamValue);

    //T2显示格式
    string GetT2String(const double fParamValue);

    //t2显示格式
    string Gett2String(const double fParamValue);

    //T3显示格式
    string GetT3String(const double fParamValue);

    //t3显示格式
    string Gett3String(const double fParamValue);

    //T4显示格式
    string GetT4String(const double fParamValue);

    //t4显示格式
    string Gett4String(const double fParamValue);

    //t5显示格式
    string Gett5String(const double fParamValue);

    //t6显示格式
    string Gett6String(const double fParamValue);

    //t.xfunit
    string GettDotString(const double ,const string );
};

#endif // TIMERESULTSTRING_H
