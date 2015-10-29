#include "numericalvalueresultstring.h"
#include <math.h>
#include <string.h>
#include "../C3I/paramstyle/paramstyleconfig.h"
extern QParamStyleConfig Paramstyleconfig;
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
    char str[128];
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
	//#域名.样式名称
    else if(showAtrriStr.find("#",0) == 0)
    {
        string str = getStyleResultString(fParamValue,showAtrriStr);
        if(str.find("{{",0) == 0)
            return str;
        else
            return getDefaultString(fParamValue);
    }
    else
        return getDefaultString(fParamValue);
}
//#域名.样式名称。通过该格式获取结果字符串，结果中包括字体、颜色、文本。
string QNumericalValueResultString::getStyleResultString(const double fParamValue,const string showAtrriStr)
{
    double value = fParamValue;
    QString resultStr = QObject::tr("");
    list<QParamStyle>* paramStyleList = Paramstyleconfig.getParamStyleList();
    list<QParamStyle>::iterator paramStyleIt = paramStyleList->begin();
	//遍历所有样式
    for(;paramStyleIt != paramStyleList->end();paramStyleIt++)
    {
        string str1 = paramStyleIt->getStyleName().toStdString();
		//比较样式名字符串和显示格式字符串
        if(strcmp(showAtrriStr.c_str(), str1.c_str()) == 0)
        {
            vector<stru_ParamStyle>* styleVector = paramStyleIt->getParamStyleVector();
            vector<stru_ParamStyle>::iterator it = styleVector->begin();
			//遍历子样式列表
            for(;it != styleVector->end();it++)
            {
                double max = ((stru_ParamStyle)*it).m_Uprlmt;
                double min = ((stru_ParamStyle)*it).m_Lorlmt;
                QString indexStr = ((stru_ParamStyle)*it).m_IndexName;
				//判断数值区间，获取样式索引名
                if(((max == min) && (min == value))||((value <= max) && (value > min)))
                {
                    list<QParamIndex>* paramIndexList = Paramstyleconfig.getParamIndexList();
                    list<QParamIndex>::iterator paramIndexIt = paramIndexList->begin();
					//通过索引名获取结果文本、字体、字符串
                    for(;paramIndexIt != paramIndexList->end();paramIndexIt++)
                    {
                        if(indexStr == paramIndexIt->getIndexName())
                        {
                            resultStr = QObject::tr("{{");
                            resultStr = resultStr + ((QParamIndex)*paramIndexIt).getFontNameStr();
                            resultStr = resultStr + QObject::tr(",");
                            resultStr = resultStr + ((QParamIndex)*paramIndexIt).getFontSizeStr();
                            resultStr = resultStr + QObject::tr(",");
                            resultStr = resultStr + ((QParamIndex)*paramIndexIt).getFontBoldStr();
                            resultStr = resultStr + QObject::tr(",");
                            resultStr = resultStr + ((QParamIndex)*paramIndexIt).getFontItalicStr();
                            resultStr = resultStr + QObject::tr("}");
                            resultStr = resultStr + QObject::tr("{");
                            resultStr = resultStr + ((QParamIndex)*paramIndexIt).getColorStr();
                            resultStr = resultStr + QObject::tr("}");
                            resultStr = resultStr + QObject::tr("{");
                            resultStr = resultStr + ((QParamIndex)*paramIndexIt).getText();
                            resultStr = resultStr + QObject::tr("}}");
                            return resultStr.toStdString();
                        }
                    }
                }
            }
        }
    }
    return resultStr.toStdString();
}

