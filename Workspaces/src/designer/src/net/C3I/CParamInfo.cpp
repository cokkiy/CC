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
#include "selfshare/src/datastruct.h"
//#include "ParamInfoTemplet.h"
//#include "ConfigInfoManager.h"



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


//参数处理类的构造函数。
CParamInfo::CParamInfo()
{
    m_pMapParamTable = new TableMap();
    m_psTaksPhaseName = NULL;
    //m_bSucC3I = true;
}

//参数处理类的析构函数，释放参数约定表map，以及每个参数占
//用的内存
CParamInfo::~CParamInfo()
{
    //析构对象时，释放参数map及参数的地址空间。
    ClearParam();

    if (m_psTaksPhaseName != NULL)
    {
        delete m_psTaksPhaseName;
        m_psTaksPhaseName = NULL;
    }
    //CParamInfoTemplet::GetParamInfoTemplet()->Realse();
}
//xhxhxhxh
//为了增加表名，增加到表号后，参数号前
//*///{++++++++ 2010-3-19 17:48:29 ++++++++
struct TableNoParam
{
    unsigned short tabNo;
    QString tabName;
};
//extern CMapWordToPtr m_tabMap;
//CMapWordToPtr m_tabMap;
// HowToUse:
//	g_a.LoadXML(str + "table_info_zx.xml");
//	g_a.LoadXML(str + "table_info_c3i.xml");

//class CAutoLoadTableXml
//{
//public:
//	CAutoLoadTableXml()
//	{
//		//LoadXML("zx_config\\sz7h\\public\\table_info_global.xml");
//	}
//	~CAutoLoadTableXml()
//	{
//		POSITION pos = m_tabMap.GetStartPosition();
//		WORD wKey=0;
//		TableNoParam* p = NULL;
//		while(pos)
//		{
//			m_tabMap.GetNextAssoc(pos, wKey, (void*&)p);
//			{
//				ASSERT(p);
//				if(p)
//				{
//					//TRACE("%d, %d, %s\n", wKey, p->tabNo, p->tabName);
//					if(p->tabName)
//						delete p->tabName;
//					delete p;
//					p=NULL;
//				}
//				m_tabMap.RemoveKey(wKey);
//				pos = m_tabMap.GetStartPosition();
//			}
//		}
//		m_tabMap.RemoveAll();
//	}
//
//	bool LoadXML(const char* sFileName)
//	{
//		// 初始化COM接口
//		CoInitialize(NULL);
//		CComPtr<IXMLDOMDocument> m_spXmlDom;
//		try
//		{
//			HRESULT hr = m_spXmlDom.CoCreateInstance(_uuidof(DOMDocument));
//			
//			if ( FAILED(hr)  || ( m_spXmlDom.p == NULL) )
//			{
//				throw "不能创建XML Parser对象";
//			}
//			// 创建成功，开始装载XML文档
//			VARIANT_BOOL bSuccess = false;
//			
//			hr = m_spXmlDom->load(CComVariant(sFileName),&bSuccess);
//			
//			if (  S_OK != hr )
//			{
//				throw"文件不存在或格式不规范";
//			}
//			
//			CComPtr<IXMLDOMNode> pNodeRoot, pNode1;
//			CComBSTR bsNodeName1;//属性名
//			CComBSTR bsText1;//文本内容
//			hr = m_spXmlDom->selectSingleNode(CComBSTR("TableInfo"), &pNodeRoot);
//			if ( S_OK != hr )
//			{
//				throw "文件不存在或格式不规范";
//			}
//			hr = pNodeRoot->get_firstChild(&pNode1);
//			if ( S_OK != hr )
//			{
//				throw "文件不存在或格式不规范";
//			}
//			while(S_OK == hr)
//			{
//				ASSERT(pNode1);
//				hr = pNode1->get_nodeName(&bsNodeName1);   //属性名
//				if((S_OK == hr) && (0 == stricmp((QString)bsNodeName1, "Table")))
//				{
//					CComPtr<IXMLDOMNode> pNode2;
//					pNode1->get_firstChild(&pNode2);
//					CComPtr<IXMLDOMNode> pNodeNext2 = NULL;//下一节点
//					ASSERT(pNode2);
//					TableNoParam* p= new TableNoParam;
//					p->tabNo = -1;
//					p->tabName = NULL;
//					while(S_OK == hr)
//					{
//						ASSERT(pNode2);
//						CComBSTR bsNodeName2;//属性名
//						hr = pNode2->get_nodeName(&bsNodeName2);   //属性名
//						if(S_OK == hr)
//						{
//							QString str = (QString)bsNodeName2;
//							str.MakeLower();
//							CComBSTR bsText;//文本内容
//							VARIANT value;//属性值
//							if(str == "tableindex")
//							{
//								CComPtr<IXMLDOMNode> pNode3 ;
//								hr = pNode2->get_firstChild(&pNode3);
//								if(S_OK == hr)
//								{
//									hr = pNode3->get_nodeValue(&value);     //文本内容
//									DWORD dw=0;
//									sscanf((QString)value.bstrVal,"%d", &dw);
//									p->tabNo = (WORD)dw;
//								}
//							}
//							else if(str == "tablename")
//							{
//								CComPtr<IXMLDOMNode> pNode3 ;
//								hr = pNode2->get_firstChild(&pNode3);
//								if(S_OK == hr)
//								{
//									hr = pNode3->get_nodeValue(&value);     //文本内容
//									QString strVal(value.bstrVal);
//									int n = strVal.GetLength();
//									if(n!=0)
//									{
//										p->tabName = new char[n+1];
//										strcpy(p->tabName, strVal);
//									}
//								}
//							}
//
//						}
//						CComPtr<IXMLDOMNode> pNodeNext2;//下一节点
//						hr = pNode2->get_nextSibling(&pNodeNext2);//取得下一节点
//						pNode2 = pNodeNext2;
//					}
//					if(p->tabNo != -1)
//					{
//						m_tabMap.SetAt(p->tabNo, p);
//					}
//					else
//					{
//						delete p;
//					}
//				}
//				CComPtr<IXMLDOMNode> pNodeNext1;//下一节点
//				hr = pNode1->get_nextSibling(&pNodeNext1);//取得下一节点
//				pNode1 = pNodeNext1;
//			}
//
//		}
//		catch(char* lpstrErr) 
//		{
//			// 出现错误
//			QString strOut;
//			strOut = (QString)lpstrErr;
//			AfxMessageBox("出现错误："+ strOut,0,0);
//			return (bool)0;
//		}
//		catch(...) 
//		{
//			// 未知错误
//			QString strOut = "未知错误...";
//			AfxMessageBox(strOut,0,0);
//			return (bool)0;
//		}
//		m_spXmlDom.Release();
//		CoUninitialize();
//		return (bool)1;
//	}
//}g_a;


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
        if (file.fileName().startsWith("zx_param_global", Qt::CaseInsensitive) 
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
                    stru_Param* pParam = new stru_Param;
                    //memset(pParam,0,sizeof(stru_Param));
                    /*状态解释不出时，显示源码
                    2012 年10月15日*/
                    //1.表号
                    pParam->usParamTable = parameter.getChildUIntValue("TableIndex");
                    //2.参数号
                    pParam->usParamCode = parameter.getChildUIntValue("ParamIndex");
                    //3.参数名字符串的存储地址
                    pParam->pcParamName = parameter.getChildValue("ParamName");
                    //4.参数代号字符串的存储地址
                    pParam->pcParamID = parameter.getChildValue("ParamCode");
                    //5.参数传输类型
                    bool bHasItem = false;
                    tempStringValue = parameter.getChildValue("TransType");
                    int nIndex = 0;
                    for (nIndex = 0; nIndex < DataTypeNum; nIndex++)
                    {
                        if (DataTypeStrings[nIndex].compare(tempStringValue, Qt::CaseInsensitive) == 0)
                        {
                            bHasItem = true;
                            break;
                        }
                    }
                    if (bHasItem)
                    {
                        pParam->ucTransType = nIndex;
                    }
                    else
                    {
                        pParam->ucTransType = 0xff;
                    }
                    //6.数长
                    pParam->usParamLen = parameter.getChildUIntValue("DataLength");
                    //7.参数转换后的存储类型
                    bHasItem = false;
                    tempStringValue = parameter.getChildValue("ConvType");
                    for (nIndex = 0; nIndex < DataTypeNum; nIndex++)
                    {
                        if (DataTypeStrings[nIndex].compare(tempStringValue, Qt::CaseInsensitive) == 0)
                        {
                            bHasItem = true;
                            break;
                        }
                    }
                    if (bHasItem)
                    {
                        pParam->ucChangeType = nIndex;
                    }
                    else
                    {
                        pParam->ucChangeType = 0xff;
                    }
                    //8.数据下限
                    pParam->dParamLorlmt = parameter.getChildDoubleValue("LowerLimit");

                    //9.数据上限
                    pParam->dParamUprlmt = parameter.getChildDoubleValue("UpperLimit");

                    //10.参数的转换系数, 量纲
                    tempStringValue = parameter.getChildValue("Dimension");
                    if (!tempStringValue.isEmpty())
                    {
                        pParam->dParamQuotiety = tempStringValue.toDouble();
                    }
                    else
                    {
                        pParam->dParamQuotiety = 1.0;
                    }

                    //11.参数单位
                    pParam->pcParamUnit = parameter.getChildValue("ParamUnit");

                    //12.参数的表现类型
                    pParam->pcShowType = parameter.getChildValue("DisplayType");

                    //12.理论值
                    pParam->pcTheoryValue = parameter.getChildValue("TheoryValue");

                    //13.范围
                    pParam->pcValueRange = parameter.getChildValue("TheoryValueRange");

                    //14.索引
                    pParam->ulGroupIndex = parameter.getChildUIntValue("GrpIndex");

                    //15.备注
                    pParam->pcParamNote = parameter.getChildValue("Remark");

                    //用于处理时间数据的单位 开始
                    //刘裕贵 2012-12-13
                    //转换格式为tp_PMTime
                    //显示格式为空，
                    //单位为s，S，秒
                    //为了使单位和量纲一致，如0.0001s表示10ms
                    //为了解决传输过程中，传输时间0.1ms和1ms单位的问题
                    if (pParam->ucChangeType == tp_PMTime || pParam->ucTransType == tp_PMTime)
                    {
                        if (pParam->pcShowType.isEmpty() && (pParam->pcParamUnit.compare("s", Qt::CaseInsensitive) == 0
                            || pParam->pcParamUnit.compare(QString::fromLocal8Bit("秒")) == 0))
                        {
                            pParam->dParamQuotiety *= 10000;
                        }
                    }
                    //用于处理时间数据的单位 结束


                    //当前值的时刻unsigned long ulParamTime;
                    pParam->ulParamTime = 0xffffffff;
                    //是否超差bool bError;
                    pParam->bError = false;
                    pParam->ucNew = 0;
                    if (pParam->ucTransType == 10 || pParam->ucTransType == 11)
                        pParam->pParamValue = new char[4 * 1024];//参数值的存放地址
                    else
                        pParam->pParamValue = new char[8];//参数值的存放地址		
                    pParam->ulParamDate = 0xffffffff;
                    //参数索引地址，当参数的表现形式为当前值对应的含义时（如0
                    //表示异常，1表示正常等）的含义表的起始地址。
                    //std::map<unsigned long, void*>* pParamIndex;	
                    this->SetParam(pParam->usParamTable, pParam->usParamCode, pParam);
                }
        }
    }
    return bResult;
}


//将参数约定表、参数值含义表及两表的关系表的XML文件装入内
//存时，每个参数都需要调用该函数装入map中。
bool CParamInfo::SetParam(unsigned short usParamTable, unsigned short usParamCode, stru_Param *pParam)
{
    //检索表号map
    TableMap::iterator  tablePos = m_pMapParamTable->find(usParamTable);
    //如果表号map有该表
    if (tablePos != m_pMapParamTable->end())
    {
        //检索该表的参数字段号map
        ParaMap* paraMap = tablePos->second;
        auto paraPos = paraMap->find(usParamCode);
        //如果该表的参数字段号map中没有该参数
        if (paraPos == paraMap->end())
        {//添加参数			
            paraMap->insert({ usParamCode,pParam });
            return true;
        }//如果该表的参数字段号map中没有该参数
        else//如果该表的参数字段号map中有该参数要返回错误
            return false;
    }//如果表号map有该表
    else//如果表号map没有该表
    {//添加表号map,并添加参数
        ParaMap* para = new ParaMap();
        para->insert({ usParamCode,pParam });//加参数
        m_pMapParamTable->insert(TableMap::value_type(usParamTable, para));//加表       
        return true;
    }
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

//数长
unsigned short CParamInfo::GetParamLen(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        //对于字符串、原码类型参数每次返回的值是不同的
        return param->usParamLen;
    else//该参数不存在返回0
        return 0;
}

//备注
QString CParamInfo::GetParamNote(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->pcParamNote;
    else//该参数不存在返回0
        return 0;
}

//数据下限
double CParamInfo::GetParamLorlmt(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->dParamLorlmt;
    else//该参数不存在返回0
        return 0;
}

//数据上限
double CParamInfo::GetParamUprlmt(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->dParamUprlmt;
    else//该参数不存在返回0
        return 0;
}



//参数的转换系数，量纲
double CParamInfo::GetParamQuotiety(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->dParamQuotiety;
    else//该参数不存在返回0
        return 0;
}

//参数代号字符串的存储地址
QString CParamInfo::GetParamID(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->pcParamID;
    else//该参数不存在返回0空地址
        return QString("");
}

//参数代号字符串的存储地址
QString CParamInfo::GetParamID(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp)
{
    /*if (pParamTemp)
    {
        if (pParamTemp->ucIndex[1] != -1)
        {
            stru_ParamInfoTemplet* pParamInfoTemplet =
                (stru_ParamInfoTemplet*)CParamInfoTemplet::GetParamInfoTemplet()->Get(
                usParamTable, usParamCode);
            if (pParamInfoTemplet)
            {
                if (pParamInfoTemplet->pParam[1])
                {
                    if (pParamTemp->ucIndex[1] >= *(unsigned short*)(pParamInfoTemplet->pParam[1]+4))
                    {
                        return GetParamID(usParamTable, usParamCode);
                    }
                    else
                    {
                        char* pTxt = pParamInfoTemplet->pParam[1] +
                            *(unsigned short*)(pParamInfoTemplet->pParam[1]+4+2+
                            2*pParamTemp->ucIndex[1]);
                        return pTxt;
                    }
                }
                else
                {
                    return GetParamID(usParamTable, usParamCode);
                }
            }
            else
            {
                return GetParamID(usParamTable, usParamCode);
            }
        }
        else
        {
            return GetParamID(usParamTable, usParamCode);
        }
    }
    else
    {
        return GetParamID(usParamTable, usParamCode);
    }*/

    return QString("");
}

//参数名字符串的存储地址
QString CParamInfo::GetParamName(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->pcParamName;
    else//该参数不存在返回0
        return QString("");
}

//参数名字符串的存储地址
QString CParamInfo::GetParamName(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp)
{
    /*if (pParamTemp)
    {
        if (pParamTemp->ucIndex[0] != -1)
        {
            stru_ParamInfoTemplet* pParamInfoTemplet =
                (stru_ParamInfoTemplet*)CParamInfoTemplet::GetParamInfoTemplet()->Get(
                usParamTable, usParamCode);
            if (pParamInfoTemplet)
            {
                if (pParamInfoTemplet->pParam[0])
                {
                    if (pParamTemp->ucIndex[0] >= *(unsigned short*)(pParamInfoTemplet->pParam[0]+4))
                    {
                        return GetParamName(usParamTable, usParamCode);
                    }
                    else
                    {
                        char* pTxt = pParamInfoTemplet->pParam[0] +
                            *(unsigned short*)(pParamInfoTemplet->pParam[0]+4+2+
                            2*pParamTemp->ucIndex[0]);
                        return pTxt;
                    }
                }
                else
                {
                    return GetParamName(usParamTable, usParamCode);
                }
            }
            else
            {
                return GetParamName(usParamTable, usParamCode);
            }
        }
        else
        {
            return GetParamName(usParamTable, usParamCode);
        }
    }
    else
    {
        return GetParamName(usParamTable, usParamCode);
    }*/

    return QString("");
}

//参数单位
QString CParamInfo::GetParamUnit(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->pcParamUnit;
    else//该参数不存在返回0
        return QString("");
}

//参数单位
QString CParamInfo::GetParamUnit(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp)
{
    /*if (pParamTemp)
    {
        if (pParamTemp->ucIndex[4] != -1)
        {
            stru_ParamInfoTemplet* pParamInfoTemplet =
                (stru_ParamInfoTemplet*)CParamInfoTemplet::GetParamInfoTemplet()->Get(
                usParamTable, usParamCode);
            if (pParamInfoTemplet)
            {
                if (pParamInfoTemplet->pParam[4])
                {
                    if (pParamTemp->ucIndex[4] >= *(unsigned short*)(pParamInfoTemplet->pParam[4]+4))
                    {
                        return GetParamUnit(usParamTable, usParamCode);
                    }
                    else
                    {
                        char* pTxt = pParamInfoTemplet->pParam[4] +
                            *(unsigned short*)(pParamInfoTemplet->pParam[4]+4+2+
                            2*pParamTemp->ucIndex[4]);
                        return pTxt;
                    }
                }
                else
                {
                    return GetParamUnit(usParamTable, usParamCode);
                }
            }
            else
            {
                return GetParamUnit(usParamTable, usParamCode);
            }
        }
        else
        {
            return GetParamUnit(usParamTable, usParamCode);
        }
    }
    else
    {
        return GetParamUnit(usParamTable, usParamCode);
    }*/
    // 	//取参数地址
    // 	stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    // 	if(param != 0)//该参数存在
    // 		return param->pcParamUnit;
    // 	else//该参数不存在返回0
    // 		return 0;

    return QString("");
}

//参数转换后的存储类型
unsigned char CParamInfo::GetChangeType(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->ucChangeType;
    else//该参数不存在返回全F
        return 0xff;
}

//参数的表现类型 表现类型是指参数在指显终端以表格方式显示时
//的显示形态。曲线绘制时不考虑此因素
QString CParamInfo::GetShowType(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->pcShowType;
    else//该参数不存在返回0
        return 0;
}
//参数的表现类型 表现类型是指参数在指显终端以表格方式显示时
//的显示形态。曲线绘制时不考虑此因素
QString CParamInfo::GetShowType(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp)
{
    /*if (pParamTemp)
    {
        if (pParamTemp->ucIndex[2] != -1)
        {
            stru_ParamInfoTemplet* pParamInfoTemplet =
                (stru_ParamInfoTemplet*)CParamInfoTemplet::GetParamInfoTemplet()->Get(
                usParamTable, usParamCode);
            if (pParamInfoTemplet)
            {
                if (pParamInfoTemplet->pParam[2])
                {
                    if (pParamTemp->ucIndex[2] >= *(unsigned short*)(pParamInfoTemplet->pParam[2]+4))
                    {
                        return GetShowType(usParamTable, usParamCode);
                    }
                    else
                    {
                        char* pTxt = pParamInfoTemplet->pParam[2] +
                            *(unsigned short*)(pParamInfoTemplet->pParam[2]+4+2+
                            2*pParamTemp->ucIndex[2]);
                        return pTxt;
                    }
                }
                else
                {
                    return GetShowType(usParamTable, usParamCode);
                }
            }
            else
            {
                return GetShowType(usParamTable, usParamCode);
            }
        }
        else
        {
            return GetShowType(usParamTable, usParamCode);
        }
    }
    else
    {
        return GetShowType(usParamTable, usParamCode);
    }*/
    return 0;
}
//参数传输类型
unsigned char CParamInfo::GetTransType(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->ucTransType;
    else//该参数不存在返回0
        return 0xff;
}

//参数索引地址，当参数的表现形式为当前值对应的含义时（如0
//表示异常，1表示正常等）的含义表的起始地址。
MeansValueMap* CParamInfo::GetParamIndex(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->pParamIndex;
    else//该参数不存在返回0
        return 0;
}

//参数索引地址，当参数的表现形式为当前值对应的含义时（如0
//表示异常，1表示正常等）的含义表的起始地址。
MeansValueMap* CParamInfo::GetParamIndex(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp)
{
    /*if (pParamTemp)
    {
        if (pParamTemp->ucIndex[3] != -1)
        {
            stru_ParamInfoTemplet* pParamInfoTemplet =
                (stru_ParamInfoTemplet*)CParamInfoTemplet::GetParamInfoTemplet()->Get(
                usParamTable, usParamCode);
            if (pParamInfoTemplet)
            {
                if (pParamInfoTemplet->pParam[3])
                {
                    if (pParamTemp->ucIndex[3] >= *(unsigned short*)(pParamInfoTemplet->pParam[3]+4))
                    {
                        return GetParamIndex(usParamTable, usParamCode);
                    }
                    else
                    {
                        char* pTxt = pParamInfoTemplet->pParam[3] +
                            *(unsigned short*)(pParamInfoTemplet->pParam[3]+4+2+
                            2*pParamTemp->ucIndex[3]);
                        if (pTxt)
                        {
                            char* pStop;
                            unsigned long ulIndex = strtoul(pTxt,&pStop,10);
                            MeansValueMap* pIndex =
                                (MeansValueMap*)(CXMLReaderParamMean::
                                GetParaMeansByGroupIndex(ulIndex));
                            return pIndex;
                        }
                        else
                        {
                            return GetParamIndex(usParamTable, usParamCode);
                        }
                    }
                }
                else
                {
                    return GetParamIndex(usParamTable, usParamCode);
                }
            }
            else
            {
                return GetParamIndex(usParamTable, usParamCode);
            }
        }
        else
        {
            return GetParamIndex(usParamTable, usParamCode);
        }
    }
    else
    {
        return GetParamIndex(usParamTable, usParamCode);
    }*/

    return 0;
}


//获得某一表号、字段号的参数地址
void* CParamInfo::GetParam(unsigned short usParamTable, unsigned short usParamCode)
{
    if (m_pMapParamTable == NULL)
    {
        return NULL;
    }

    // first,find in cache
    unsigned int key = usParamTable;
    key = key << 16;
    key += usParamCode;
    auto c_pos = cachedParam.find(key);
    if (c_pos != cachedParam.end())
    {
        // find in cache
        return c_pos->second;
    }
    else
    {
        stru_Param* result = NULL;
        //检索表号map
        TableMap::iterator pos =
            m_pMapParamTable->find(usParamTable);
        if (pos != m_pMapParamTable->end())//如果表号map有该表
        {
            //检索该表的参数字段号map
            ParaMap* code = pos->second;
            auto param_pos = code->find(usParamCode);
            if (param_pos != code->end())//有该参数
                result= param_pos->second;
//             else//没有该参数返回0空地址
//                 return 0;
        }//如果表号map有该表
//         else//没有该表返回0空地址	
//             return 0;
//             save to cache
        cachedParam.insert({key,result});

        return result;
    }

    return NULL;
}



//析构对象时，释放参数map及参数的地址空间。
void CParamInfo::ClearParam()
{
    TableMap::iterator tablePos = m_pMapParamTable->begin();
    while (tablePos != m_pMapParamTable->end())
    {
        //得到表
        TableMap* pTableMap = (TableMap*)((*tablePos).second);
        if (pTableMap != NULL)
        {
            TableMap::iterator codePos = pTableMap->begin();
            while (codePos != pTableMap->end())
            {
                //得到参数
                stru_Param* pParam = (stru_Param*)((*codePos).second);
                if (pParam != NULL)
                {

                    if (pParam->pParamValue != NULL)
                    {
                        delete pParam->pParamValue;//参数值的存放地址
                        pParam->pParamValue = NULL;
                    }


                    if (pParam->pParamIndex != NULL)
                    {
                        MeansValueMap::iterator paramIndexPos =
                            pParam->pParamIndex->begin();
                        while (paramIndexPos != pParam->pParamIndex->end())
                        {
                            MeanValue * pMeanVale = (MeanValue *)(*paramIndexPos).second;
                            if (pMeanVale != NULL)
                            {
                                char* pChar = pMeanVale->content;
                                if (pChar != NULL)
                                {
                                    delete pChar; pChar = NULL;
                                }
                                delete pMeanVale; pMeanVale = NULL;
                            }
                            paramIndexPos++;
                        }
                        pParam->pParamIndex->clear();
                        //delete pParam->pParamIndex;pParam->pParamIndex = NULL;
                    }
                    delete pParam;
                    pParam = NULL;
                }
                codePos++;
            }
            pTableMap->clear();
            delete pTableMap;
        }
        tablePos++;
    }
    m_pMapParamTable->clear();
    delete m_pMapParamTable;
    m_pMapParamTable = NULL;
}


unsigned short CParamInfo::GetNextParamTable(unsigned long next)
{
    TableMap::iterator pos =
        m_pMapParamTable->begin();
    if (pos == m_pMapParamTable->end())
        return 0;
    for (unsigned long ul = 0; ul < next; ul++)
    {
        pos++;
        if (pos == m_pMapParamTable->end())
            return 0;
    }
    return(unsigned short)(*pos).first;
}

unsigned short CParamInfo::GetNextParamCode(unsigned short usTable, unsigned long next)
{
    TableMap::iterator pos =
        m_pMapParamTable->find(usTable);

    if (pos == m_pMapParamTable->end())
        return 0;

    TableMap* code =
        (TableMap*)((*pos).second);
    pos = code->begin();

    if (pos == code->end())
        return 0;
    for (unsigned long ul = 0; ul < next; ul++)
    {
        pos++;
        if (pos == code->end())
            return 0;
    }
    return(unsigned short)(*pos).first;
}

void CParamInfo::ClearBuf()
{
    TableMap::iterator tablePos = m_pMapParamTable->begin();
    while (tablePos != m_pMapParamTable->end())
    {
        //得到表
        TableMap* pTableMap = (TableMap*)((*tablePos).second);
        if (pTableMap != NULL)
        {
            TableMap::iterator codePos = pTableMap->begin();
            while (codePos != pTableMap->end())
            {
                //得到参数
                stru_Param* pParam = (stru_Param*)((*codePos).second);
                if (pParam != NULL)
                {
                    pParam->ulParamTime = 0xffffffff;
                    pParam->ulParamDate = 0xffffffff;
                    pParam->ucNew = 0;
                }
                codePos++;
            }
        }
        tablePos++;
    }
}

void CParamInfo::ChangeParamFile(QString str)
{
    //xhxhxhxh
    //2010.03.31
    //将zx_param_global_zx.xml和zx_param_global_c3i.xml等文件的数据
    //保存到param_c3i.dat之中
    //////////////////////////////////////////////////

    //CFile fC3I;
    //unsigned long ul = 0;
    //if(fC3I.Open((char*)(const char *)(str + "param_c3i.dat"),
    //	CFile::modeCreate|CFile::modeWrite) )
    //{
    //	TableMap::iterator tablePos = m_pMapParamTable->begin();
    //	while(tablePos != m_pMapParamTable->end())
    //	{
    //		//得到表
    //		TableMap* pTableMap = (TableMap*)((*tablePos).second);
    //		if(pTableMap != NULL)
    //		{
    //			TableMap::iterator codePos = pTableMap->begin();
    //			while(codePos != pTableMap->end())
    //			{
    //				//得到参数
    //				stru_Param* pParam = (stru_Param*)((*codePos).second);
    //				if(pParam != NULL)
    //				{
    //					fC3I.Write(pParam, sizeof(stru_Param));
    //					ul = 0;
    //					if (pParam->pcParamID)
    //					{
    //						ul += (strlen(pParam->pcParamID)+1);
    //					}
    //					else
    //					{
    //						ul++;
    //					}
    //					if (pParam->pcParamName)
    //					{
    //						ul += (strlen(pParam->pcParamName)+1);
    //					}
    //					else
    //					{
    //						ul++;
    //					}
    //					if (pParam->pcParamNote)
    //					{
    //						ul += (strlen(pParam->pcParamNote)+1);
    //					}
    //					else
    //					{
    //						ul++;
    //					}
    //					if (pParam->pcParamUnit)
    //					{
    //						ul += (strlen(pParam->pcParamUnit)+1);
    //					}
    //					else
    //					{
    //						ul++;
    //					}
    //					if (pParam->pcShowType)
    //					{
    //						ul += (strlen(pParam->pcShowType)+1);
    //					}
    //					else
    //					{
    //						ul++;
    //					}
    //					if (pParam->pcTheoryValue)
    //					{
    //						ul += (strlen(pParam->pcTheoryValue)+1);
    //					}
    //					else
    //					{
    //						ul++;
    //					}
    //					if (pParam->pcValueRange)
    //					{
    //						ul += (strlen(pParam->pcValueRange)+1);
    //					}
    //					else
    //					{
    //						ul++;
    //					}
    //					fC3I.Write(&ul, sizeof(unsigned long));
    //					if (pParam->pcParamID)
    //					{
    //						fC3I.Write(pParam->pcParamID, strlen(pParam->pcParamID)+1);
    //					}
    //					else
    //					{
    //						fC3I.Write("", 1);
    //					}
    //					if (pParam->pcParamName)
    //					{
    //						fC3I.Write(pParam->pcParamName, strlen(pParam->pcParamName)+1);
    //					}
    //					else
    //					{
    //						fC3I.Write("", 1);
    //					}
    //					if (pParam->pcParamNote)
    //					{
    //						fC3I.Write(pParam->pcParamNote, strlen(pParam->pcParamNote)+1);
    //					}
    //					else
    //					{
    //						fC3I.Write("", 1);
    //					}
    //					if (pParam->pcParamUnit)
    //					{
    //						fC3I.Write(pParam->pcParamUnit, strlen(pParam->pcParamUnit)+1);
    //					}
    //					else
    //					{
    //						fC3I.Write("", 1);
    //					}
    //					if (pParam->pcShowType)
    //					{
    //						fC3I.Write(pParam->pcShowType, strlen(pParam->pcShowType)+1);
    //					}
    //					else
    //					{
    //						fC3I.Write("", 1);
    //					}
    //					if (pParam->pcTheoryValue)
    //					{
    //						fC3I.Write(pParam->pcTheoryValue, strlen(pParam->pcTheoryValue)+1);
    //					}
    //					else
    //					{
    //						fC3I.Write("", 1);
    //					}
    //					if (pParam->pcValueRange)
    //					{
    //						fC3I.Write(pParam->pcValueRange, strlen(pParam->pcValueRange)+1);
    //					}
    //					else
    //					{
    //						fC3I.Write("", 1);
    //					}
    //					
    //					pParam->pParamIndex = (MeansValueMap*)(
    //						CXMLReaderParamMean::GetParaMeansByGroupIndex(pParam->ulGroupIndex));		
    //				}
    //				codePos++;
    //			}			
    //		}
    //		tablePos++;
    //	}
    //	fC3I.Close();
    //}
}

bool CParamInfo::IsTable(unsigned short usTable)
{
    //检索表号map
    TableMap::iterator pos =
        m_pMapParamTable->find(usTable);
    if (pos != m_pMapParamTable->end())//如果表号map有该表
        return true;//如果表号map有该表
    else//没有该表		
        return false;
}

QString CParamInfo::GetParamTheoryValue(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->pcTheoryValue;
    else//该参数不存在返回0
        return 0;
}

QString CParamInfo::GetParamTheoryValue(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp)
{
    /*if (pParamTemp)
    {
        if (pParamTemp->ucIndex[5] != -1)
        {
            stru_ParamInfoTemplet* pParamInfoTemplet =
                (stru_ParamInfoTemplet*)CParamInfoTemplet::GetParamInfoTemplet()->Get(
                usParamTable, usParamCode);
            if (pParamInfoTemplet)
            {
                if (pParamInfoTemplet->pParam[5])
                {
                    if (pParamTemp->ucIndex[5] >= *(unsigned short*)(pParamInfoTemplet->pParam[5]+4))
                    {
                        return GetParamTheoryValue(usParamTable, usParamCode);
                    }
                    else
                    {
                        char* pTxt = pParamInfoTemplet->pParam[5] +
                            *(unsigned short*)(pParamInfoTemplet->pParam[5]+4+2+
                            2*pParamTemp->ucIndex[5]);
                        return pTxt;
                    }
                }
                else
                {
                    return GetParamTheoryValue(usParamTable, usParamCode);
                }
            }
            else
            {
                return GetParamTheoryValue(usParamTable, usParamCode);
            }
        }
        else
        {
            return GetParamTheoryValue(usParamTable, usParamCode);
        }
    }
    else
    {
        return GetParamTheoryValue(usParamTable, usParamCode);
    }*/
    return QString("");
}

QString CParamInfo::GetParamValueRange(unsigned short usParamTable, unsigned short usParamCode)
{
    //取参数地址
    stru_Param* param = (stru_Param*)GetParam(usParamTable, usParamCode);
    if (param != 0)//该参数存在
        return param->pcValueRange;
    else//该参数不存在返回0
        return 0;
}

QString CParamInfo::GetParamValueRange(unsigned short usParamTable, unsigned short usParamCode, stru_ParamTemp* pParamTemp)
{
    /*if (pParamTemp)
    {
        if (pParamTemp->ucIndex[6] != -1)
        {
            stru_ParamInfoTemplet* pParamInfoTemplet =
                (stru_ParamInfoTemplet*)CParamInfoTemplet::GetParamInfoTemplet()->Get(
                usParamTable, usParamCode);
            if (pParamInfoTemplet)
            {
                if (pParamInfoTemplet->pParam[6])
                {
                    if (pParamTemp->ucIndex[6] >= *(unsigned short*)(pParamInfoTemplet->pParam[6]+4))
                    {
                        return GetParamValueRange(usParamTable, usParamCode);
                    }
                    else
                    {
                        char* pTxt = pParamInfoTemplet->pParam[6] +
                            *(unsigned short*)(pParamInfoTemplet->pParam[6]+4+2+
                            2*pParamTemp->ucIndex[6]);
                        return pTxt;
                    }
                }
                else
                {
                    return GetParamValueRange(usParamTable, usParamCode);
                }
            }
            else
            {
                return GetParamValueRange(usParamTable, usParamCode);
            }
        }
        else
        {
            return GetParamValueRange(usParamTable, usParamCode);
        }
    }
    else
    {
        return GetParamValueRange(usParamTable, usParamCode);
    }*/
    return QString("");
}

