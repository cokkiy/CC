#ifndef ABSTRACTPARAM_H
#define ABSTRACTPARAM_H
#include "net_global.h"
#include <qvariant.h>
#include <string>
using namespace std;
//参数支持的数据类型
typedef enum DataType
{
    tp_char,tp_BYTE,tp_short,tp_WORD,tp_long,tp_DWORD,
    tp_float,tp_double,tp_PMTime,tp_Date,tp_String,tp_Code,
    tp_BCDTime,tp_UTCTime,tp_ulong8,tp_long8,tp_longE,
    DataTypeNum
}DataType;
//数据类型字符串数组
const string DataTypeStrings[DataTypeNum] = {
    "char","byte","short","word","long","dword",
    "float","double","pmtime","date","string","code",
    "bcdtime","utctime","ulong8","long8","longe" };

/* 	0:char-1字节有符号整数
    1:unsigned char--1字节无符号整数
    2:short--2字节有符号短整
    3:WORD-2字节无符号短整
    4:long-4字节有符号长整
    5:DWORD-4字节无符号长整
    6:float-4字节浮点数
    7:double-8字节双精度浮点数
    8:PMTime-4字节无符号整数（量化单位0.1ms）
    9:Data－4字节无符号整数（依照4.3.9.5.6节日期量表示方法）
    10:String－字符串数据类型，表示方法如下。字符串数据由"字符串长度"、
    "字符串有效数据区"和"字符串结束尾"三部分构成。"字符串长度"占2个字节，
    表示"字符串有效数据区"的长度，不包括本身和字符串结束尾，采用2字节无
    符号整数表示，取值范围1～4000；"字符串有效数据区"存储有效字符，其长
    度由"字符串长度"决定，采用ANSI（英文字符采用ASCII编码，中文字符采用
    GB2312编码）编码；"字符串结束尾"为1个字节00h，"字符串有效数据区"中
    不得出现00f字符。
    11:Code－原码数据类型，表示方法如下。原码数据由"原码长度"、"原码有效数
    据区"和"原码结束尾"三部分构成。"原码长度"占2个字节，表示"原码有效数
    据区"的长度，不包括本身和字符串结束尾，采用2字节无符号整数表示，取
    值范围1～4000；"原码有效数据区"存储有效的二进制原码，其长度由"原码
    长度"决定；"原码结束尾"为1个字节ffh。
    12:""－表示空
*/

class NETSHARED_EXPORT AbstractParam
{
public:
    enum ParamStatus
    {
        First,
        Old,
        New
    };
public:
    AbstractParam();
    ~AbstractParam();
    //是否可以进行公式计算
    bool canCompute();
    //从源码获取参数的浮点数的值
    bool getValueFromCode(double&);
    //获取参数的变体的值
    bool getValue(QVariant&);
    //设置参数的浮点数的值
    bool setValue(double val);
    //获取参数的浮点数的值
    bool getValue(double& val);
    //获取参数的小写的源码
    bool getCode(std::string & result);
    //复制函数
    AbstractParam & operator =(const AbstractParam &aParam);
public:
    //表号
    unsigned short m_tableNo;
    //参数号
    unsigned short m_paramNo;
    //参数名称
    string m_paramName;
    //参数代号
    string m_paramCode;
    //参数传输类型
    unsigned char m_transType;
    //参数数据长度
    unsigned short m_paramDataLen;
    //参数转换类型
    unsigned char m_changeType;
    //数据下限
    double m_paramLorlmt;
    //数据上限
    double m_paramUprlmt;
    //参数的转换系数，量纲
    //用来对网络接收的数据进行处理
    double m_paramQuotiety;
    //参数单位
    string m_paramUnit;
    //参数的表现类型 表现类型是指参数在指显终端以表格方式显示时
    //的显示形态。曲线绘制时不考虑此因素
    string m_dispType;
    //理论值
    string m_theoryValue;
    //理论值范围
    string m_valueRange;
    //索引
    unsigned int m_ulGroupIndex;
    //备注
    string m_pcParamNote;
private:
    //是否为新值,对于源码、字符串类型参数该值始终为0
    //0表示首次，1表示旧值，2表示新值
    unsigned char m_ucNew;
    //参数的数据存放空间的指针
    void * m_paramVal;
    //参数采集日期
    unsigned int   m_paramDate;
    //参数采集时间,绝对时
    unsigned int   m_paramTime;
    //参数数据类型
    string m_paramDataType;
    //参数存放的浮点型的数据
    double m_val;
public:
    //设置参数表号
    void SetTableNo(const unsigned short& bh);
    //获取参数表号
    unsigned short GetTableNo();

    //设置参数号
    void SetParamNo(const unsigned short& bh);
    //获取参数号
    unsigned short GetParamNo();

    //设置参数名称
    void SetParamName(const string& title);
    //获取参数名称
    string GetParamName();

    //设置参数代号
    void SetParamCode(const string& code);
    //获取参数代号
    string GetParamCode();

    //设置参数传输类型
    void SetParamTranType(const unsigned char& vr);
    //获取参数传输类型
    unsigned char GetParamTranType();

    //设置参数长度
    void SetParamDataLen(const unsigned short& len);
    //获取参数长度
    unsigned short GetParamDataLen();

    //设置参数转换类型
    void SetParamConvType(unsigned char type);
    //获取参数转换类型
    unsigned char GetParamConvType();

    //设置参数下限
    void SetParamLorlmt(const double& ll);
    //获取参数下限
    double GetParamLorlmt();

    //设置参数上限
    void SetParamUprlmt(const double& ul);
    //获取参数上限
    double GetParamUprlmt();

    //设置参数量纲
    void SetParamQuotiety(const double& qt);
    //获取参数量纲
    double GetParamQuotiety();

    //设置参数单位
    void SetParamUnit(const string& unit);
    //获取参数单位
    string GetParamUnit();

    //设置参数显示类型
    void SetParamShowType(const string& showtype);
    //获取参数显示类型
    string GetParamShowType();    

    //设置参数理论值
    void SetParamTheoryValue(const string& tv);
    //获取参数理论值
    string GetParamTheoryValue();

    //设置参数显示范围
    void SetParamTheoryRange(const string& vr);
    //获取参数显示范围
    string GetParamTheoryRange();

    //设置参数索引
    void SetParamGroupIndex(unsigned int gi);
    //获取参数索引
    unsigned int GetParamGroupIndex();

    //设置参数备注
    void SetParamNote(const string& note);
    //获取参数备注
    string GetParamNote();
public:
    void SetParamStatus(unsigned char ucNew);
    unsigned int GetParamStatus();

    void SetParamDate(unsigned int date);
    unsigned int GetParamDate();

    void SetParamTime(unsigned int time);
    unsigned int GetParamTime();

    //根据数据长度填写参数数据的指针
    bool GetFrameData(void *des);
    //释放参数所占用的空间,准备容纳另一参数
    void FreeParamSpace();
    //初始化参数的存放空间
    void InitParamSpace();
    //获取参数的数据存放空间的指针
    void* GetParamValuePoint();
    //获取参数类型的字符串
    static string GetTypeString(unsigned char type);

};

#endif // ABSTRACTPARAM_H
