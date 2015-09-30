#ifndef DATERESULTSTRING_H
#define DATERESULTSTRING_H

#include "dataresultstring.h"
#include<stdio.h>
#include<string>

using namespace std;
class QDateResultString
{
public:
    QDateResultString();
    //库尔勒LED日期 (XXXX年XX月XX日XX时XX分XX秒)
    string getLEDDateString(const double fParamValue);
   //积日 2000年1月1日
    string getDayofYearString(const double fParamValue);
    string GetDateString(const double Value,const string type);
};

#endif // DATERESULTSTRING_H
