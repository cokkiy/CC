// LFormula.h: interface for the LFormula class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(PARAMFORMULA_H)
#define PARAMFORMULA_H
#include <vector>
#include <stack>
#include <exception>
#include <string>
#include "abstractparam.h"
#include "selfshare/src/config/config.h"
#include "net_global.h"
#include <qvariant.h>
using namespace std;

#if defined(Q_OS_WIN) && !defined(Q_CC_GNU)
#  define Q_INT64_C(c) c ## i64    /* signed 64 bit constant */
#  define Q_UINT64_C(c) c ## ui64   /* unsigned 64 bit constant */
typedef __int64 qint64;            /* 64 bit signed */
typedef unsigned __int64 quint64;  /* 64 bit unsigned */
#else
#  define Q_INT64_C(c) static_cast<long long>(c ## LL)     /* signed 64 bit constant */
#  define Q_UINT64_C(c) static_cast<unsigned long long>(c ## ULL) /* unsigned 64 bit constant */
typedef long long qint64;           /* 64 bit signed */
typedef unsigned long long quint64; /* 64 bit unsigned */
#endif
#define INDEX(tn,pn) (((unsigned int)tn<<16)|(unsigned short)pn)
#define TABNO(tp) ((unsigned short)((unsigned int)tp>>16))
#define PARAMNO(tp) ((unsigned short)tp)
typedef unsigned short WORD;
typedef std::map<unsigned int,AbstractParam>  FormulaZXParamMap;
typedef std::map<unsigned int,list<HistoryParam>> TempHistoryParamMap;

enum OPERATORDATAType
{
	SEMICOLON,//;
	BRACKET3_R,//}
    BRACKET2_R,//]
	BRACKET_R,//)
	COMMA,//,
	FUN,//fun_type
	BRACKET_L,//(
	BRACKET2_L,//[
	BRACKET3_L,//{
	ADD,//加法
	SUB,//减法
	MUL,//乘法
	DIV,//除法
	AND,//与运算
	OR,//或运算
	SHIFT_RIGHT,//右移运算
	SHIFT_LEFT,//左移运算
	DATA,//数据
	PARAM,//参数
};
//   ;   }   ]   )   ,  fun  (   [    {   +   -   *    /

enum FunName
{
	JT0,//减T0参数
	JD1,//-90°~ 270°
	JD2,//-180°~ 180°
	JD3,//0°~ 360°
	YCC,//设置遥测无效值
	ABS,//绝对值
	DJS,//倒计时
	SET,//根据值设置参数值
	TIM//取参数时间
};
enum ParseType
{
    HistoryData,
    CurData
};
enum GetHistoryDataType
{
    Time,
    Pos
};

struct OperatorData 
{
    char type;
	union{
		double data;
        AbstractParam* param;
		FunName funname;
    };
    bool GetValue(double& ret_d);
    bool GetValue(QVariant& ret_d);
    // 获取参数的小写的源码
    bool GetCode(std::string & code);
	void GetCodeValue(double& ret_d);
};
#define MINIMUSDATA (0.0000000000001)
#define ERRORPARAM "获取参数值失败"
#define INVALIDPARAM "无效参数"
#define INVALIDDATATYPE "无效数据类型"
#define ERRORFUNNAME "非法函数名"
#define JT0INVALIDPARAM "JT0函数的参数无效"
#define INVALIDOPERATORCOMB "非法的操作符组合"
#define INVALIDOPERATOR "有不认识的操作符"
#define UNHANDLEOPERATORCOMB "操作符组合的关系值无效"
#define NOENOUGHFUNPARAM "函数参数数量不够"
#define NOSETVALUE "不设参数值"
#define NODISPLAY "不显示参数"

class NETSHARED_EXPORT LFormula
{
public:
    LFormula(Config*);
    virtual ~LFormula();
    /*!
      通过输入公式化的参数表达式获取计算结果
      @param string formula  字符串，包含参数的公式计算表达式
      @param double& result  公式计算的结果，为浮点数
      @return int  获取成功或失败，1为成功，-1为失败
    */
    int compute(std::string formula,double& result);
    /*!
      通过输入公式化的参数表达式获取计算结果
      @param string formula  字符串，包含参数的公式计算表达式
      @param QVariant& result  公式计算的结果，为浮点数
      @return int  获取成功或失败，1为成功，-1为失败
    */
    int compute(std::string formula,QVariant& result);
    /*!
      通过输入公式化的参数表达式获取历史数据的计算结果
      @param string formula  字符串，包含参数的公式计算表达式
      @param QVector<double>& result 历史数据的计算结果，为浮点数的QVector
      @return int  获取成功或失败，1为成功，-1为失败
    */
    int compute(std::string formula,QVector<double>& result);
    /*!
      通过输入公式化的参数表达式获取历史数据的计算结果
      @param string formula  字符串，包含参数的公式计算表达式
      @param QVector<double>& result 历史数据的计算结果，为浮点数的QVector
      @param QVector<unsigned int>& time 历史数据的计算结果，为浮点数的QVector
      @return int  获取成功或失败，1为成功，-1为失败
    */
    int compute(std::string formula,QVector<double>& result,QVector<unsigned int>& time);
    /*!
      通过输入公式化的参数表达式获取历史数据的计算结果
      @param string formulaX  字符串，包含参数的公式计算表达式
      @param string formulaY  字符串，包含参数的公式计算表达式
      @param QVector<double>& resultX 历史数据的计算结果，为浮点数的QVector
      @param QVector<double>& resultY 历史数据的计算结果，为浮点数的QVector
      @return int  获取成功或失败，1为成功，-1为失败
    */
    int compute(std::string formulaX,std::string formulaY,QVector<double>& resultX,QVector<double>& resultY);
    /*!
      通过输入公式化的参数表达式获取参数数据的大写的源码
      @param string formula  字符串，包含参数的公式计算表达式
      @param std::string& code  参数数据的源码
      @return int  获取成功或失败，1为成功，-1为失败
    */
    int code(std::string formula,std::string& code);
    /*!
      获取参数数据中的表号和参数号
      @param string formula  字符串，包含参数的公式计算表达式
      @param unsigned short &tn  表号
      @param unsigned short &pn  参数号
      @return bool  获取成功或失败，true为成功，false为失败
    */
    static bool getParam(std::string formula,unsigned short &tn,unsigned short & pn);
    /*!
      经过公式计算后，获取参数的指针
      @return AbstractParam*   参数的指针
    */
    AbstractParam* getParam();

    /*
     *获得时标
    */
    int getDate();
    int getTime();

    /*
     *设置时标
    */
    void setDate(int date);
    void setTime(int time);
    //获取历史数据的位置
    size_t getPos();
    //设置历史数据的位置
    void setPos(size_t pos);
private:
    /*!
      更新公式中包含参数的数据
    */
    void updateParam();
    /*!
      获取表达式的值
      @param double& result  公式计算的结果，为浮点数
      @return bool  获取成功或失败，true为成功，false为失败
    */
    bool compute(double& result);
    /*!
      获取表达式的值,可用于获取字符串和原码等类型的数据
      @param QVariant& result  公式计算的结果，为变体
      @return bool  获取成功或失败，true为成功，false为失败
    */
    bool compute(QVariant& result);
    /*!
      解释公式
      @param string formula  字符串，包含参数的公式计算表达式
      @return bool  获取成功或失败，true为成功，false为失败
    */
    bool parse(std::string formula,ParseType type = CurData);
    /*!
      是否是只包含单个参数的公式，如[1,2]
      @return bool  true,只包含单个参数;false,还有其他公式项目
    */
    bool singleParam();
private:
    //是否能使用公式
    bool use(){return m_bUse;}
    // no use
    bool change(std::string);
	//是否使用公式
    bool m_bUse;
    //是否是单个参数
    bool m_bSingleParam;
	//字符串公式
    std::string m_formula;
	//操作数对象列表
    std::vector< OperatorData > m_OperData;

    int m_savTime;

    int m_savDate;

    size_t m_curPos;
private:
    //所有参数配置信息
    Config* m_config;
    FormulaZXParamMap m_zxparamMap;
    TempHistoryParamMap m_tHistoryMap;
    AbstractParam* m_primaryParam;
    AbstractParam* addParam(unsigned short,unsigned short,ParseType type = CurData);
    AbstractParam* getParam(std::string formula);
    //no use
    void updateParam(unsigned short,unsigned short);    
    /*!
      更新公式中包含参数的数据
    */
    bool insertOneHistroyParam();
    /*!
      更新公式中包含参数的数据
    */
    bool insertOneHistroyParam(unsigned int& time);
    /*!
      更新公式中包含参数的数据
    */
    bool updateAllHistroyParam(GetHistoryDataType type = Pos);
    //no use
    int getParamCount();
private:	
	//解释数据
    bool ParseData(std::stack<OperatorData >& ostack,std::string data,OperatorData& Od);
	//处理操作符
	//运用操作关系表进行操作符的处理
    bool HandleOperator(std::stack<OperatorData >& ostack,const OperatorData& od);
	//处理最后剩下的操作符
	void HandleOperator(std::stack<OperatorData >& ostack);
    //绝对时减去T0时间 得到相对时#include "selfshare/src/config/config.h"
	void fun_JT0(OperatorData a, OperatorData b,double &ret);
	//正常的俯仰角度值转换到-90°~ 270°之间
	void fun_JD1(OperatorData a,double &ret);
	//方位角度值转换到-180°~ 180°之间
	void fun_JD2(OperatorData a,double &ret);
	//方位角度值转换到0°~ 360°之间
	void fun_JD3(OperatorData a,double &ret);
	//遥测参数的值无效时不显示
	void fun_YCC(OperatorData a, OperatorData b,double &ret);
	//加法运算
    bool fun_ADD(OperatorData a,OperatorData b,double &ret);
	//减法运算
    bool fun_SUB(OperatorData a,OperatorData b,double &ret);
	//乘法运算
	void fun_MUL(OperatorData a,OperatorData b,double &ret);
	//除法运算
    bool fun_DIV(OperatorData a,OperatorData b,double &ret);
	//与运算
	void fun_AND(OperatorData a,OperatorData b,double &ret);
	//或运算
	void fun_OR(OperatorData a,OperatorData b,double &ret);
	//右移运算
	void fun_SHIFT_RIGHT(OperatorData a,OperatorData b,double &ret);
	//左移运算
	void fun_SHIFT_LEFT(OperatorData a,OperatorData b,double &ret);
	//绝对值
	void fun_ABS(OperatorData a,double &ret);
	//计算倒计时
	void fun_DJS(OperatorData a,double &ret);
	//设置参数值,判断参数judge如果等于常量cont，则设置源参数sour的值，否则不更新值
	void fun_SET(OperatorData sour,OperatorData judge,OperatorData cont,double &ret);
	//参数sour是源参数，取该参数的参数时间
    bool fun_TIM(OperatorData sour,double &ret);
};

#endif // !defined(PARAMFORMULA_H)
