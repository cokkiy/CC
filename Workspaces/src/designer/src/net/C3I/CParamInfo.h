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
#include <mutex>
#include "../selfshare/src/config/config.h"
#include "../selfshare/src/zxparambuffer/zxparambuffer.h"
//该类用于对C3I参数的处理，在进程初始化时利用该类对象装在参数
//进内存，通过该类可以获取某一表号、字段号参数的所有信息。
class CParamInfo  
{
public:
	//进程初始化时，将参数约定表、参数值含义表及两表的关系表的
	//XML文件装入内存，并调用SetParam装入map中。
	bool LoadParamFromXML(const QString& dir);

	//参数处理类的构造函数。
	CParamInfo();
	//参数处理类的析构函数，释放参数约定表map，以及每个参数占
	//用的内存
	virtual ~CParamInfo();
	
	//获得某一表号、字段号的参数地址
    AbstractParam* GetParam(unsigned short usParamTable, unsigned short usParamCode);

	//将参数约定表、参数值含义表及两表的关系表的XML文件装入内	
	//存时，每个参数都需要调用该函数装入map中。
    bool SetParam(unsigned short usParamTable, unsigned short usParamCode, AbstractParam& pParam);
    /*!
    获取全部信息约定表
    @return TableMap*
    作者：cokkiy（张立民)
    创建时间：2015/09/28 10:13:59
    */
    TableMap* getTableMap();
	
protected:

    //参数约定表
	TableMap* m_pMapParamTable;

    // 缓存的表号,参数号<->参数列表
    CacheTableMap cachedParam;

private:
    //mutex for reading and writing cache
    mutex lockForCache;
    // current reading
    bool bRead = false;
    // current writing
    bool bWrite = false;
};



