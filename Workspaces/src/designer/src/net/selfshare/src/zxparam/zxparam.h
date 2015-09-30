// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：zxparam.h
// 文件摘要：指显系统参数类头文件，用来声明参数类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef ZXPARAM_H
#define ZXPARAM_H
#include "../../../net_global.h"
#include "../../../main/src/parameter/parameter.h"

using namespace std;

class NETSHARED_EXPORT ZXParam : public Parameter
{
	public:
		//构造函数
		ZXParam();

		//拷贝构造函数
		ZXParam(const ZXParam &a);	

		// 赋值函数
		ZXParam & operator =(const ZXParam &a);									

		//析构函数
		~ZXParam();

		//从信息约定表获取参数相关信息
		bool GetZXParamInfo(unsigned short tableno, unsigned short paramno);

		//设置参数日期
		void SetZXParamDate(unsigned int& ddate);

		//获取参数日期
		unsigned int GetZXParamDate();

		//设置参数超差
		void SetZXParamError(bool& error);

		//获取参数超差
		unsigned int GetZXParamError();

		//设置参数是否为新值
        void SetZXParamNew(unsigned char n);

		//获取参数是否为新值标识
		unsigned int GetZXParamNew();

		//设置参数原始值
		void SetZXParamOldValue(const void* pValue);

		//获取参数原始值
		void GetZXParamOldValue(unsigned char* pValue);

		//设置参数备注
		void SetZXParamNote(const string& note);

		//获取参数备注
		string GetZXParamNote();

		//设置参数下限
		void SetZXParamLorlmt(const double& ll);

		//获取参数下限
		double GetZXParamLorlmt();

		//设置参数上限
		void SetZXParamUprlmt(const double& ul);

		//获取参数上限
		double GetZXParamUprlmt();

		//设置参数量纲
		void SetZXParamQuotiety(const double& qt);

		//获取参数量纲
		double GetZXParamQuotiety();

		//设置参数代号
		void SetZXParamCode(const string& code);

		//获取参数代号
		string GetZXParamCode();

		//设置参数单位
		void SetZXParamUnit(const string& unit);

		//获取参数单位
		string GetZXParamUnit();

		//设置参数显示类型
		void SetZXParamShowType(const string& dt);

		//获取参数显示类型
		string GetZXParamShowType();

		//设置参数显示类型
		void SetZXParamTheoryValue(const string& tv);

		//获取参数显示类型
		string GetZXParamTheoryValue();

		//设置参数显示范围
		void SetZXParamTheoryRange(const string& vr);

		//获取参数显示范围
		string GetZXParamTheoryRange();

		//设置参数索引
		void SetZXParamGroupIndex(unsigned long& gi);

		//获取参数索引
		unsigned long GetZXParamGroupIndex();

        unsigned char GetParamTranType();

        unsigned char GetParamConvType();
    public:
		//参数名称
        //string m_paramTitle;

		unsigned int m_paramdate;

		//是否超差
		bool m_bError;

		//是否为新值,对于源码、字符串类型参数该值始终为0
		//0表示首次，1表示旧值，2表示新值
		unsigned char m_ucNew;

		//原始值，不包括：原码和字符串
		unsigned char m_ucParamValue_Old[8];

		//备注
		string m_pcParamNote;

		//数据下限
		double m_dParamLorlmt;

		//数据上限
		double m_dParamUprlmt;

		//参数的转换系数，量纲
		//用来对网络接收的数据进行处理
		double m_dParamQuotiety;

		//参数代号字符串的存储地址
		string m_pcParamID;

		//参数单位
		string m_pcParamUnit;

		//参数转换后的存储类型
		unsigned char m_ucChangeType;

		//参数的表现类型 表现类型是指参数在指显终端以表格方式显示时
		//的显示形态。曲线绘制时不考虑此因素
		string m_pcShowType;

		//参数传输类型
		unsigned char m_ucTransType;

		//理论值
		string m_pcTheoryValue;

		//理论值范围
		string m_pcValueRange;

		//索引
		unsigned long m_ulGroupIndex;


};

#endif
