#include "coderesultstring.h"
#include <QString>
CodeResultString::CodeResultString()
{
}
string CodeResultString::getResultString(string codeStr,string dispStr)
{
    if(!strcmp(dispStr.c_str(), "h"))
    {
        return codeStr;
    }
    else if(!strcmp(dispStr.c_str(), "b"))
    {
        string t_str = "";
        foreach(char t_c,codeStr)
        {
            switch(t_c)
            {
            case '0':
                t_str += "0000";
                break;
            case '1':
                t_str += "0001";
                break;
            case '2':
                t_str += "0010";
                break;
            case '3':
                t_str += "0011";
            case '4':
                t_str += "0100";
                break;
            case '5':
                t_str += "0101";
                break;
            case '6':
                t_str += "0110";
                break;
            case '7':
                t_str += "0111";
                break;
            case '8':
                t_str += "1000";
                break;
            case '9':
                t_str += "1001";
                break;
            case 'a':
                t_str += "1010";
                break;
            case 'b':
                t_str += "1011";
                break;
            case 'c':
                t_str += "1100";
                break;
            case 'd':
                t_str += "1101";
                break;
            case 'e':
                t_str += "1110";
                break;
            case 'f':
                t_str += "1111";
                break;
            }
        }
        return t_str;

    }
    else //if(!strcmp(m_ShowAttrString.c_str(), "H"))
    {
        QString t_qStr = codeStr.c_str();
        return t_qStr.toUpper().toStdString();
    }
}
