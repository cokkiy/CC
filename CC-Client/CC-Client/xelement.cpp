// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：xelement.cpp
// 文件摘要：XElement类实现，实现了xml元素结构
//          实现了查找子元素和获取元素属性的各种方法
//
// 原始版本：1.0
// 作   者：张立民
// 完成日期：2015-08-26
//**************************************************************//
#include "xelement.h"
#include <QStringList>
#include <QDebug>

XElement::XElement()
{
}

//添加一个属性到属性列表
void XElement::addAttribute(const XAttribute & attribute)
{
    list<XAttribute>::iterator theIterator;

    bool hasSame=false;
    for(theIterator=this->Attributes.begin();theIterator!=this->Attributes.end();theIterator++)
    {
        if(0==theIterator->Name.compare(attribute.Name,Qt::CaseInsensitive))
        {
            hasSame=true;
            break;
        }
    }

    if(!hasSame)
    {
        //检查是否是元素的Id或者是元素的值
        if(attribute.Name.compare("id",Qt::CaseInsensitive)==0)
        {
            this->Id=attribute.Value;
        }
        else if(attribute.Name.compare("value",Qt::CaseInsensitive)==0)
        {
            this->Value=attribute.Value;
        }
        else
        {
            this->Attributes.push_back(attribute);
        }        

    }
}

//添加一个子元素到子元素列表
void XElement::addChildElement(const XElement & element)
{
    this->childElements.push_back(element);
}

//获取第一个子节点
XElement XElement::getFirstChildElement() const
{
    return this->childElements.front();
}

//查找符合指定名称和位置的子节点
void XElement::findElement(QStringList::const_iterator& strIterator, list<XElement>& result,
                           const list<XElement>& children, const QStringList& nameList) const
{
    //分析名称,"网卡设置@2"分为"网卡设置"和2
    QStringList nameCountpair=strIterator->split("@",QString::SkipEmptyParts);
    QString _name=nameCountpair.front();
    int _count=1;
    int index=1;
    if(nameCountpair.count()==2)
    {
        _name=nameCountpair.front();
        _count=nameCountpair.back().toInt();
    }    

    list<XElement>::const_iterator eIterator;

    for(eIterator = children.begin();
        eIterator != children.end(); eIterator++)
    {
        //名称相同
        if(eIterator->Name.compare(_name,Qt::CaseInsensitive)==0)
        {
            if(index<_count)
            {
                index++;
                continue;
            }
            //且在指定位置
            else if(index==_count)
            {
                if(strIterator==(nameList.end()-1))
                {
                    //到最后一级了（到“本机地址”）了
                    result.push_back(*eIterator);
                }
                else
                {
                    //还在上级（“网卡设置”节点），继续在本级的下级找
                    strIterator++;

                    if(strIterator!=nameList.end())
                    {
                        findElement(strIterator, result, eIterator->childElements, nameList);
                    }

                    index++;
                }                
            }
            else //index > _count
            {
                break;
            }
        }
    }
}

//************************************
// 方法:    getChildDoubleValue
// 可访问性:    private 
// 返回值:   double
// 参数: const QString & childName
// 说明：获取指定名称子节点的double类型的值，如果有多个相同名称的字节点，则指返回第一个节点的值
//          如果指定名称的节点不存在，则返回0.0
// 返回值说明：指定名称子节点的double类型的值
// 作者：cokkiy（张立民)
// 创建时间：2015/09/21 15:38:03
//************************************
double XElement::getChildDoubleValue(const QString& childName) const
{
    ElementList children = this->getChildrenByName(childName);
    if (children.empty())
        return 0;

    QString v = ((XElement)children.front()).Value;
    
    return v.toDouble();
}

//************************************
// 方法:    getChildUIntValue
// 可访问性:    private 
// 返回值:   uint
// 参数: QString childName
// 说明：获取指定名称子节点的unsigned int类型值，如果有多个相同名称的字节点，则指返回第一个节点的值
//          如果指定名称的节点不存在，则返回0,数据值支持后缀M,MB和K,KB
// 返回值说明：指定名称子节点的unsigned int类型的值
// 作者：cokkiy（张立民)
// 创建时间：2015/09/21 11:54:19
//************************************
uint XElement::getChildUIntValue(const QString& childName) const
{
    ElementList children = this->getChildrenByName(childName);
    if (children.empty())
        return 0;

    QString v = ((XElement)children.front()).Value;
    if (v.endsWith('M', Qt::CaseInsensitive) || v.endsWith("MB", Qt::CaseInsensitive))
    {
        //MB
        int endIndex = v.indexOf("m", 0, Qt::CaseInsensitive);
        QString digit = v.left(endIndex);
        return digit.toUInt() * 1024 * 1024;
    }
    else if (v.endsWith('k', Qt::CaseInsensitive) || v.endsWith("kb", Qt::CaseInsensitive))
    {
        //KB
        int endIndex = v.indexOf("k", 0, Qt::CaseInsensitive);
        QString digit = v.left(endIndex);
        return digit.toUInt() * 1024;
    }
    return v.toUInt();
}

//获取指定名称子节点的bool类型值，如果有多个相同名称的字节点，则指返回第一个节点的值
//如果指定名称的节点不错在，则返回false
// 如果节点值为字符串"true"或1,返回true,如果节点值为字符串"false"或0,返回false
// childName支持以“/"分割的多级元素，具体见2）getChildrenByName说明
bool XElement::getChildBoolValue(const QString childName) const
{
    ElementList children = this->getChildrenByName(childName);
    if (children.empty())
        return false;

    QString v = ((XElement)children.front()).Value;
    if (v.compare("true", Qt::CaseInsensitive) == 0 || v.compare("1") == 0)
    {
        return true;
    }

    return false;
}

//获取指定名称的子元素
//支持以“/"分割的多级元素，如”网卡设置/本机地址“表示获取当前节点下的”网卡设置”下的
//"本机地址"子节点,如果本级下面有多个"网卡设置"设置子节点，则只获取第一个"网卡设置"下的
//也可以用"网卡设置@2/本机地址"指定获取第二个"网卡设置"下的"本机地址"子节点
list<XElement> XElement::getChildrenByName(const QString& name) const
{
    list<XElement> result;

    //把字符串”网卡设置/本机地址“或”网卡设置@2/本机地址“分拆
    QStringList nameList = name.split("/",QString::SkipEmptyParts);
    QStringList::const_iterator strIterator=nameList.begin();


    //根据名称和位置查找子节点
    findElement(strIterator, result, this->childElements, nameList);


    return result;
}

//获取指定名称子节点的值，如果有多个相同名称的字节点，则指返回第一个节点的值
//如果指定名称的节点不错在，则返回空字符串(“”)
QString XElement::getChildValue(const QString& childName) const
{
    ElementList children=this->getChildrenByName(childName);
    if(children.empty())
        return QStringLiteral("");

    return ((XElement)children.front()).Value;
}

//获取指定名称子节点的int类型值，如果有多个相同名称的字节点，则指返回第一个节点的值
//如果指定名称的节点不存在，则返回-1,数据值支持后缀M,MB和K,KB
int XElement::getChildIntValue(const QString& childName) const
{
    ElementList children=this->getChildrenByName(childName);
    if(children.empty())
        return -1;

    QString v= ((XElement)children.front()).Value;
    if (v.endsWith('M', Qt::CaseInsensitive) || v.endsWith("MB", Qt::CaseInsensitive))
    {
        //MB
        int endIndex = v.indexOf("m", 0, Qt::CaseInsensitive);
        QString digit = v.left(endIndex);
        return digit.toInt() * 1024 * 1024;
    }
    else if (v.endsWith('k', Qt::CaseInsensitive) || v.endsWith("kb", Qt::CaseInsensitive))
    {
        //KB
        int endIndex = v.indexOf("k", 0, Qt::CaseInsensitive);
        QString digit = v.left(endIndex);
        return digit.toInt() * 1024;
    }
    return v.toInt();
}

//获取本节点指定名称属性的属性值
//如果属性不存在，返回空字符串
QString XElement::getAttributeValue(const QString& attrName) const
{
    list<XAttribute>::const_iterator i=this->Attributes.begin();
    for(;i!=this->Attributes.end();i++)
    {
        if(i->Name.compare(attrName,Qt::CaseInsensitive)==0)
        {
            return i->Value;
        }
    }

    return "";
}


//获取指定id的子元素
list<XElement> XElement::getChildrenById(const QString& id) const
{
    list<XElement> children;
    list<XElement>::const_iterator eIterator;

    for(eIterator = this->childElements.begin();
        eIterator != this->childElements.end(); eIterator++)
    {
        if(eIterator->Name.compare(id,Qt::CaseInsensitive)==0)
        {
            children.push_back(*eIterator);
        }
    }
    return children;
}
