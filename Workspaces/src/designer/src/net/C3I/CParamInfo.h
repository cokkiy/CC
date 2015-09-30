// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：CParamInfo.h
// 文件摘要：该文件是称输入对话框类体定义
//  
// 原始版本：
// 作    者：谢虎
// 完成日期：2009年8月21日
//  
// *************************** 修改记录 ************************** //
// 版    本：
// 修 改 者：
// 完成日期：
// 修改内容：
// 
// *************************************************************** //
// 

#pragma once

#include <map>
#include "stru.h"

//定义ParaMap类型,表示某表号下的所有参数集合(按参数号组织)
typedef std::map<unsigned short, stru_Param*> ParaMap;
//定义ParaMap类型,表示按表号,参数号组织的所有参数集合
typedef std::map<unsigned short, ParaMap*> TableMap;

class CXMLReaderParamMean;
/*class CXMLReaderColor;*/
//该类用于对C3I参数的处理，在进程初始化时利用该类对象装在参数
//进内存，通过该类可以获取某一表号、字段号参数的所有信息。
class CParamInfo  
{
public:
	bool IsTable(unsigned short usTable);
	void ClearBuf();
	
	unsigned short GetNextParamCode(unsigned short usTable, unsigned long next);
	unsigned short GetNextParamTable(unsigned long next);
	//数长
	unsigned short GetParamLen(unsigned short usParamTable, unsigned short usParamCode);

	//备注
    QString GetParamNote(unsigned short usParamTable, unsigned short usParamCode);

	//数据下限
	double GetParamLorlmt(unsigned short usParamTable, unsigned short usParamCode);

	//数据上限
	double GetParamUprlmt(unsigned short usParamTable, unsigned short usParamCode);

	

	//参数的转换系数，量纲
	double GetParamQuotiety(unsigned short usParamTable, unsigned short usParamCode);

	//参数代号字符串的存储地址
    QString GetParamID(unsigned short usParamTable, unsigned short usParamCode);

	//参数代号字符串的存储地址
	QString GetParamID(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp);

	//参数名字符串的存储地址
    QString GetParamName(unsigned short usParamTable, unsigned short usParamCode);
	
	//参数名字符串的存储地址
    QString GetParamName(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp);

	//参数单位
    QString GetParamUnit(unsigned short usParamTable, unsigned short usParamCode);

	//参数单位
    QString GetParamUnit(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp);

	//参数转换后的存储类型
	unsigned char GetChangeType(unsigned short usParamTable, unsigned short usParamCode);

	//参数的表现类型 表现类型是指参数在指显终端以表格方式显示时
	//的显示形态。曲线绘制时不考虑此因素
    QString GetShowType(unsigned short usParamTable, unsigned short usParamCode);

	//参数的表现类型 表现类型是指参数在指显终端以表格方式显示时
	//的显示形态。曲线绘制时不考虑此因素
    QString GetShowType(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp);

	//参数传输类型
	unsigned char GetTransType(unsigned short usParamTable, unsigned short usParamCode);

	//参数索引地址，当参数的表现形式为当前值对应的含义时（如0
	//表示异常，1表示正常等）的含义表的起始地址。	
	MeansValueMap* GetParamIndex(unsigned short usParamTable, unsigned short usParamCode);

	//参数索引地址，当参数的表现形式为当前值对应的含义时（如0
	//表示异常，1表示正常等）的含义表的起始地址。	
	MeansValueMap* GetParamIndex(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp);


	//参数理论值。	
    QString GetParamTheoryValue(unsigned short usParamTable, unsigned short usParamCode);
	//参数理论值。	
    QString GetParamTheoryValue(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp);

	//pcValueRange
	//参数值范围。	
    QString GetParamValueRange(unsigned short usParamTable, unsigned short usParamCode);
	//参数值范围。	
    QString GetParamValueRange(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp);

	//进程初始化时，将参数约定表、参数值含义表及两表的关系表的
	//XML文件装入内存，并调用SetParam装入map中。
	bool LoadParamFromXML(const QString& dir);

	//参数处理类的构造函数。
	CParamInfo();
	//参数处理类的析构函数，释放参数约定表map，以及每个参数占
	//用的内存
	virtual ~CParamInfo();
	
	//获得某一表号、字段号的参数地址
	void* GetParam(unsigned short usParamTable, unsigned short usParamCode);

	//将参数约定表、参数值含义表及两表的关系表的XML文件装入内	
	//存时，每个参数都需要调用该函数装入map中。
	bool SetParam(unsigned short usParamTable, unsigned short usParamCode, stru_Param* pParam);
	char* m_psTaksPhaseName;


    /*!
    获取全部信息约定表
    @return TableMap*
    作者：cokkiy（张立民)
    创建时间：2015/09/28 10:13:59
    */
    TableMap* getTableMap();
	
protected:
	void ChangeParamFile(QString str);
	//析构对象时，释放参数map及参数的地址空间。
	void ClearParam();

    //参数约定表
	TableMap* m_pMapParamTable;
};



