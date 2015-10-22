#include "informationinstance.h"
#include "selfshare/src/config/config.h"
#include "main/src/network/network.h"
#include "netinstance.h"
#include "LFormula.h"
#include "C3I/CParamInfoRT.h"

Config g_cfg;
//外部变量,C3I/PDXP包处理对象
extern CParamInfoRT g_paramInfoRT;

InformationInstance::InformationInstance(QObject *parent) :
    InformationInterface(parent)
{
}
int InformationInstance::load(QString dir)
{
    /*std::string temp = dir.toStdString();
    if (g_cfg.LoadConfig(temp.c_str()) == false)
    {
        cout << "加载信息处理配置信息失败，请检查配置重启软件！" << endl;
        return -1;
    }

    getParamMap();*/
    return 1;
}
/*!
  获取所有表的表名
  @return TabList  所有表的表名列表
*/
TabMap InformationInstance::getTabList()
{
    map<unsigned short, string>::iterator theIterator;
    for(theIterator = g_cfg.m_tableInfo.begin();theIterator != g_cfg.m_tableInfo.end();theIterator++)
    {
        m_tabMap[(*theIterator).first] = (*theIterator).second.c_str();
    }

    return m_tabMap;
}

/*!
  获取所有参数的映射
  @return ParamMap  所有参数的映射
*/
ParamMap InformationInstance::getParamMap()
{
    TableMap* tablemap = g_paramInfoRT.getTableMap();
    for (auto& table : *tablemap)
    {
        unsigned short tabNo=table.first;
        for (auto& para : table.second)
        {
            unsigned short paraNo = para.first;
            m_paramMap[tabNo][paraNo] = para.second;
        }

    }
    
//     WholeTable::iterator theTIterator;
//     ParameterTable::iterator thePIterator;
//     AbstractParam tempParam;
//     tempParam.InputInfoTable(&g_cfg.m_infoTabProc);
// 
//     for(theTIterator = g_cfg.m_infoTabProc.GetNormalTab()->begin();
//         theTIterator != g_cfg.m_infoTabProc.GetNormalTab()->end();
//         theTIterator++)
//         for(thePIterator = (*theTIterator).second.m_parameterTable.begin();
//             thePIterator != (*theTIterator).second.m_parameterTable.end();
//             thePIterator++
//             )
//         {
//             if(tempParam.GetZXParamInfo((*theTIterator).first, (*thePIterator).first) == true)
//             {
//                 m_paramMap[(*theTIterator).first][(*thePIterator).first] = tempParam;
//                 tempParam.FreeParamSpace();
//             }
//         }
    return m_paramMap;
}



/*!
获取指定表号的参数（数组）
@param unsigned short tabNo 表号
@return SingleTableParamVector tabNo指定的表号的参数
作者：cokkiy（张立民)
创建时间：2015/09/25 18:03:08
*/
SingleTableParamVector InformationInstance::getParamVector(unsigned short tabNo)
{
    SingleTableParamVector tmpParamVector;   
    
    if(m_paramMap.isEmpty())
    {
        getParamMap();
    }
    ParamMap::iterator it = m_paramMap.find(tabNo);
    if(it!=m_paramMap.end())
    {
        foreach (AbstractParam tmpParam, it.value()) {
            tmpParamVector.push_back(tmpParam);
        }
    }
    return tmpParamVector;
}


AbstractParam* InformationInstance::getParam(unsigned short tabNo,unsigned short paramNo)
{
    //TODO: 修改这个地方,从ParamInfoRT中获取配置信息
    if(m_paramMap.isEmpty())
    {
        getParamMap();
    }
    ParamMap::iterator t_tabIt;
    t_tabIt = m_paramMap.find(tabNo);
    if(t_tabIt != m_paramMap.end())
    {
        QMap<unsigned short, AbstractParam>::iterator t_paramIt;
        t_paramIt = t_tabIt.value().find(paramNo);
        if(t_paramIt != t_tabIt.value().end())
        {
            return &t_paramIt.value();
        }
    }
    return NULL;
}


AbstractParam* InformationInstance::getParam(QString data)
{
    unsigned short tabNo,paramNo;
    if(LFormula::getParam(data.toStdString(),tabNo,paramNo))
    {
        return getParam(tabNo,paramNo);
    }
    return NULL;
}

