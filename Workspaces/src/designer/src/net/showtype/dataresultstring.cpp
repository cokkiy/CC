#include "dataresultstring.h"
#include "numericalvalueresultstring.h"
#include "timeresultstring.h"
#include "dateresultstring.h"
#include "coderesultstring.h"
QDataResultString::QDataResultString(LFormula * formula,const string computeStr,const string dispStr)
    :m_formula(formula),m_computeStr(computeStr),m_paramShowAttrString(dispStr)
{
}

string QDataResultString::getResultString()
{
    analysisShowAttrString();
    string str = "";
    switch (m_ShowAttrType) {
    case Value:
    {
        if(m_formula != NULL&&m_formula->compute(m_computeStr,m_paramValue)!=-1)
        {
            QNumericalValueResultString NumericalValueResultString;
            str = NumericalValueResultString.getNumericalValueString(m_paramValue,m_ShowAttrString);
        }
        break;
    }
    case Time:
    {
        if(m_formula != NULL&&m_formula->compute(m_computeStr,m_paramValue)!=-1)
        {
            QTimeResultString TimeResultString;
            str = TimeResultString.GetTimeString(m_paramValue,m_ShowAttrString);
        }
        break;
    }
    case Date:
    {
        if(m_formula != NULL&&m_formula->compute(m_computeStr,m_paramValue)!=-1)
        {
            QDateResultString DateResultString;
            str = DateResultString.GetDateString(m_paramValue,m_ShowAttrString);
        }
        break;
    }
    case Code:
    {
        if(m_formula != NULL&&m_formula->code(m_computeStr,str)!=-1)
        {
            CodeResultString codeResultString;
            str = codeResultString.getResultString(str,m_ShowAttrString);
        }
        break;
    }
    case Other:
    default:
    {
        QVariant result;
        if(m_formula != NULL&&m_formula->compute(m_computeStr,result)!=-1)
        {
            switch(result.type())
            {
            case QVariant::Double:

            {
                m_paramValue = result.value<double>();
                AbstractParam* param = m_formula->getParam();
                //获取参数的转换类型，如果转换类型没有定义则采用传输类型,andrew,20150929
                m_paramDataType = param->GetParamConvType();
                if(m_paramDataType == -1)
                {
                    m_paramDataType = param->GetParamTranType();
                }
                str = GetDefaultString(m_paramValue);
            }
                break;
            case QVariant::ByteArray:
            {
                QByteArray ba = result.value<QByteArray>();
                str = ba.toHex().toUpper().data();
            }
                break;
            case QVariant::String:
                str = result.value<QString>().toStdString();
                break;
            }
        }
        break;
    }
  }
    return str;
}

string QDataResultString::GetDefaultString(const double Value)
{
    char str[128];
    switch (m_paramDataType) {
    case tp_char:
        {
            char chValue = (char)Value;
            sprintf(str,"%d",chValue);
            break;
         }
    case tp_BYTE:
        {
            unsigned char byteValue = (unsigned char)Value;
            sprintf(str,"%d",byteValue);
            break;
        }
    case tp_short:
         {
            short shortValue = (short)Value;
            sprintf(str,"%d",shortValue);
            break;
         }
    case tp_WORD:
         {
            ushort ushortValue = (ushort)Value;
            sprintf(str,"%d",ushortValue);
            break;
         }
    case tp_long:
        {
            long longValue = (long)Value;
            sprintf(str,"%d",longValue);
            break;
        }
    case tp_DWORD:
        {
            uint uintValue = (uint)Value;
            sprintf(str,"%d",uintValue);
            break;
        }
   case tp_float:
         {
            float floatValue = (float)Value;
            sprintf(str,"%.3f",floatValue);
            break;
         }
   case tp_double:
         {
            sprintf(str,"%.3f",Value);
            break;
         }
   case tp_PMTime:
         {
            sprintf(str,"%.3f",Value/10000);
            break;
          }
   case tp_Date:
         {
            float floatValue = (float)Value;
            sprintf(str,"%.3f",floatValue);
            break;
          }

    case tp_BCDTime:
        {
            float floatValue = (float)Value;
            sprintf(str,"%.3f",floatValue);
            break;
        }
    case tp_UTCTime:
        {
            float floatValue = (float)Value;
            sprintf(str,"%.3f",floatValue);
            break;
        }
    case tp_ulong8:
        {
            float floatValue = (float)Value;
            sprintf(str,"%.3f",floatValue);
            break;
        }
    case tp_long8:
        {
            float floatValue = (float)Value;
            sprintf(str,"%.3f",floatValue);
            break;
        }
    case tp_longE:
        {
            float floatValue = (float)Value;
            sprintf(str,"%.3f",floatValue);
            break;
        }
    default:
        {
            sprintf(str,"%.3f",Value);
            break;
        }
    }
    return str;
}
void QDataResultString::analysisShowAttrString()
{
    m_ShowAttrString = m_paramShowAttrString;
    if(m_paramShowAttrString.length() == 0)
         m_ShowAttrString = m_paramShowType;
    m_ShowAttrType = 0;
    if(!strcmp(m_ShowAttrString.c_str(), "H"))
    {
        m_ShowAttrType = Code;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "h"))
    {
        m_ShowAttrType = Code;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "b"))
    {
        m_ShowAttrType = Code;
        return;
    }
    if(m_ShowAttrString.find('.',0) == 0)
    {
        m_ShowAttrType = Value;
        return;
    }
    if(m_ShowAttrString.find('?',0) == 0)
    {
        m_ShowAttrType = Value;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "T1"))
    {
        m_ShowAttrType = Time;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "t1"))
    {
        m_ShowAttrType = Time;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "T2"))
    {
        m_ShowAttrType = Time;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "t2"))
    {
        m_ShowAttrType = Time;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "T3"))
    {
        m_ShowAttrType = Time;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "t3"))
    {
        m_ShowAttrType = Time;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "T4"))
    {
        m_ShowAttrType = Time;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "t4"))
    {
        m_ShowAttrType = Time;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "t5"))
    {
        m_ShowAttrType = Time;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "t6"))
    {
        m_ShowAttrType = Time;
        return;
    }
    if(m_ShowAttrString.find("t.",0) == 0)
    {
        m_ShowAttrType = Time;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "LED_DT"))
    {
        m_ShowAttrType = Date;
        return;
    }
    if(!strcmp(m_ShowAttrString.c_str(), "d2000"))
    {
        m_ShowAttrType = Date;
        return;
    }
    m_ShowAttrType = Other;
}

