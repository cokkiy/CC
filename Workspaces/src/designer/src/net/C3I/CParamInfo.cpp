// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：CParamInfo.cpp
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

#include <QDir>
#include <QDebug>
#include "CParamInfo.h"
#include  "selfshare/src/config/xdocument.h"
//外部变量.配置信息和缓存信息
extern Config g_cfg;

//参数处理类的构造函数。
CParamInfo::CParamInfo()
{
    m_pMapParamTable = g_cfg.m_zxParamBuf.GetAllParamMapPoint();
}

//参数处理类的析构函数，释放参数约定表map，以及每个参数占
//用的内存
CParamInfo::~CParamInfo()
{

}
//xhxhxhxh
//为了增加表名，增加到表号后，参数号前
//*///{++++++++ 2010-3-19 17:48:29 ++++++++
struct TableNoParam
{
    unsigned short tabNo;
    QString tabName;
};

//************************************
// 方法:    LoadParamFromXML
// 可访问性:    public 
// 返回值:   bool
// 参数: QString dir
// 说明：装载参数约定表到内存，并调用SetParam装入map中。
// 返回值说明：如果装载成功,返回true,如果装载过程中发生错误,返回false
// 作者：cokkiy（张立民)
// 创建时间：2015/09/21 11:23:42
//************************************
bool CParamInfo::LoadParamFromXML(const QString& dir)
{
    bool bResult = true;

    //目录
    QDir qDir(dir);
    QFileInfoList fileList = qDir.entryInfoList();
    QString tempStringValue = "";

    for (auto file : fileList)
    {
        //**********装载zx_param_global_c3i.xml等文件***//
        if (file.fileName().startsWith("zx_param_global_", Qt::CaseInsensitive) 
            && file.fileName().endsWith(".xml", Qt::CaseInsensitive))
        {
               XDocument doc;
               if (!doc.Load(file.absoluteFilePath()))
               {
                   qWarning() << QObject::tr("Load file: %1 failure.").arg(file.absoluteFilePath());
                   bResult = false;
               }
                ElementList parameters = doc.getChild().getChildrenByName("Parameter");

                for (XElement parameter : parameters)
                {
                    AbstractParam pParam;
                    //1.表号
                    pParam.SetTableNo(parameter.getChildUIntValue("TableIndex"));
                    //2.参数号
                    pParam.SetParamNo(parameter.getChildUIntValue("ParamIndex"));
                    //3.参数名字符串的存储地址
                    pParam.SetParamName(parameter.getChildValue("ParamName").toStdString());
                    //4.参数代号字符串的存储地址
                    pParam.SetParamCode(parameter.getChildValue("ParamCode").toStdString());
                    //5.参数传输类型
                    tempStringValue = parameter.getChildValue("TransType");
                    for (int nIndex = 0; nIndex < DataTypeNum; nIndex++)
                    {
                        if (DataTypeStrings[nIndex].compare(tempStringValue.toLower().toStdString()) == 0)
                        {
                            pParam.SetParamTranType(nIndex);
                            break;
                        }
                    }
                    
                    //6.数长
                    pParam.SetParamDataLen(parameter.getChildUIntValue("DataLength"));
                    //7.参数转换后的存储类型
                    tempStringValue = parameter.getChildValue("ConvType");
                    for (int nIndex = 0; nIndex < DataTypeNum; nIndex++)
                    {
                        if (DataTypeStrings[nIndex].compare(tempStringValue.toLower().toStdString()) == 0)
                        {
                            pParam.SetParamConvType(nIndex);
                            break;
                        }
                    }
                    
                    //8.数据下限
                    pParam.SetParamLorlmt(parameter.getChildDoubleValue("LowerLimit"));

                    //9.数据上限
                    pParam.SetParamUprlmt(parameter.getChildDoubleValue("UpperLimit"));

                    //10.参数的转换系数, 量纲
                    tempStringValue = parameter.getChildValue("Dimension");
                    if (!tempStringValue.isEmpty())
                    {
                        pParam.SetParamQuotiety(tempStringValue.toDouble());
                    }
                    else
                    {
                        pParam.SetParamQuotiety(1.0);
                    }

                    //11.参数单位
                    pParam.SetParamUnit(parameter.getChildValue("ParamUnit").toStdString());

                    //12.参数的表现类型
                    pParam.SetParamShowType(parameter.getChildValue("DisplayType").toStdString());

                    //12.理论值
                    pParam.SetParamTheoryValue(parameter.getChildValue("TheoryValue").toStdString());

                    //13.范围
                    pParam.SetParamTheoryRange(parameter.getChildValue("TheoryValueRange").toStdString());

                    //14.索引
                    pParam.SetParamGroupIndex(parameter.getChildUIntValue("GrpIndex"));

                    //15.备注
                    pParam.SetParamNote(parameter.getChildValue("Remark").toStdString());

                    pParam.InitParamSpace();

                    SetParam(pParam.GetTableNo(), pParam.GetParamNo(), pParam);
                }
        }
    }
    return bResult;
}


//将参数约定表、参数值含义表及两表的关系表的XML文件装入内
//存时，每个参数都需要调用该函数装入map中。
bool CParamInfo::SetParam(unsigned short usParamTable, unsigned short usParamCode, AbstractParam &pParam)
{
    (*m_pMapParamTable)[usParamTable][usParamCode] = pParam;
    return true;
}

/*!
获取全部的信息约定表
@return TableMap
作者：cokkiy（张立民)
创建时间：2015/09/28 10:12:44
*/
TableMap* CParamInfo::getTableMap()
{
    return m_pMapParamTable;
}

//获得某一表号、字段号的参数地址
AbstractParam* CParamInfo::GetParam(unsigned short usParamTable, unsigned short usParamCode)
{
    if (m_pMapParamTable == NULL)
    {
        return NULL;
    }

    // first,find in cache
    unsigned int key = usParamTable;
    key = key << 16;
    key += usParamCode;    
    while (bWrite){} //wait untile write finished
    bRead = true; //start reading
    auto c_pos = cachedParam.find(key);
    auto end = cachedParam.end();
    bRead = false;
    if (c_pos != end)
    {
        // find in cache
        return c_pos->second;
    }
    else
    {
        AbstractParam* param = NULL;
        //检索表号map
        TableMap::iterator pos =
            m_pMapParamTable->find(usParamTable);
        if (pos != m_pMapParamTable->end())//如果表号map有该表
        {
            //检索该表的参数字段号map
            ParaMap& code = pos->second;
            auto param_pos = code.find(usParamCode);
            if (param_pos != code.end())//有该参数
                param= &(param_pos->second);
        }//如果表号map有该表

         //save to cache
        lockForCache.lock();
        bWrite = true;
        while (bRead) {  } //wait until current reading finished
        cachedParam.insert({key,param});
        bWrite = false;
        lockForCache.unlock();

        return param;
    }

    return NULL;
}
