// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：xelement.h
// 文件摘要：XElement类定义，定义了xml元素结构
//          定义了查找子元素和获取元素属性的各种方法
// 使用方法：
//        0)获取节点值
//        Value属性返回节点值
//
//        1）获取该节点的第一个子节点
//        XElement getFirstChildElement() const;
//
//        2）获取该节点下的指定名字的子节点
//        list<XElement> getChildrenByName(const QString& /*name*/) const;
//        说明：
//           支持以“/"分割的多级元素，如”网卡设置/本机地址“表示获取当前节点下的”网卡设置”下的
//           "本机地址"子节点,如果本级下面有多个"网卡设置"设置子节点，则只获取第一个"网卡设置"下的
//           也可以用"网卡设置@2/本机地址"指定获取第二个"网卡设置"下的"本机地址"子节点
//
//        3）获取该节点下的指定Id的子节点
//        list<XElement> getChildrenById(const QString& /*id*/) const;
//
//        4)获取指定名称字节点的值，如果有多个相同名称的字节点，则指返回第一个节点的值
//        如果指定名称的节点不错在，则返回空字符串(“”)
//        QString getChildValue(const QString& childName);
//        说明：childName支持以“/"分割的多级元素，具体见2）说明
//
//        5)获取指定名称字节点的int类型值，如果有多个相同名称的字节点，则指返回第一个节点的值
//        如果指定名称的节点不错在，则返回-1, 数据值支持后缀M,MB和K,KB
//        int XElement::getChildIntValue(const QString& childName) const;
//        说明：childName支持以“/"分割的多级元素，具体见2）说明
//
//        5)获取本节点指定名称属性的属性值
//        如果属性不存在，返回空字符串
//        QString getAttributeValue(QString /*attrName=属性值*/);
// 原始版本：1.0
// 作   者：张立民
// 完成日期：2015-08-26
//**************************************************************//
#ifndef XELEMENT_H
#define XELEMENT_H
#include <QString>
#include <list>
#include <QStringList>
#include "xattribute.h"

using namespace std;

//定义了xml文档元素
class XElement
{

private:
    //查找指定名称和位置的子节点
    void findElement(QStringList::const_iterator &strIterator, std::list<XElement>& result,
        const std::list<XElement> &children, const QStringList& nameList) const;


public:
    XElement();

public:
    //元素名称
    QString Name;

    //元素值
    QString Value;

    //元素Id
    QString Id;

    //子节点列表
    std::list<XElement> childElements;

    //元素的属性列表
    std::list<XAttribute> Attributes;


    //获取第一个子元素
    XElement getFirstChildElement() const;

    //获取指定名字的子节点
    //支持以“/"分割的多级元素，如”网卡设置/本机地址“表示获取当前节点下的”网卡设置”下的
    //"本机地址"子节点,如果本级下面有多个"网卡设置"设置子节点，则只获取第一个"网卡设置"下的
    //也可以用"网卡设置@2/本机地址"指定获取第二个"网卡设置"下的"本机地址"子节点
    std::list<XElement> getChildrenByName(const QString& /*name*/) const;

    //获取指定名称字节点的，如果有多个相同名称的字节点，则指返回第一个节点的值
    //如果指定名称的节点不错在，则返回空字符串(“”)
    //childName支持以“/"分割的多级元素，具体见2）getChildrenByName说明
    QString getChildValue(const QString& childName) const;

    //获取指定名称字节点的int类型值，如果有多个相同名称的字节点，则指返回第一个节点的值
    //如果指定名称的节点不错在，则返回-1,数据值支持后缀M,MB和K,KB
    // childName支持以“/"分割的多级元素，具体见2）getChildrenByName说明
    int getChildIntValue(const QString& childName) const;

    //获取指定名称字节点的uint类型值，如果有多个相同名称的字节点，则指返回第一个节点的值
    //如果指定名称的节点不错在，则返回0,数据值支持后缀M,MB和K,KB
    // childName支持以“/"分割的多级元素，具体见2）getChildrenByName说明
    uint getChildUIntValue(const QString& childName) const;

    //获取指定名称子节点的bool类型值，如果有多个相同名称的字节点，则指返回第一个节点的值
    //如果指定名称的节点不错在，则返回false
    // 如果节点值为字符串"true"或1,返回true,如果节点值为字符串"false"或0,返回false
    // childName支持以“/"分割的多级元素，具体见2）getChildrenByName说明
    bool getChildBoolValue(const QString childName) const;

    //获取指定名称字节点的uint类型值，如果有多个相同名称的字节点，则指返回第一个节点的值
    //如果指定名称的节点不错在，则返回0.0
    // childName支持以“/"分割的多级元素，具体见2）getChildrenByName说明
    double getChildDoubleValue(const QString& childName) const;

    //获取指定Id的子节点
    std::list<XElement> getChildrenById(const QString& /*id*/) const;

    //获取本节点指定名称属性的属性值
    //如果属性不存在，返回空字符串
    QString getAttributeValue(const QString& /*attrName=属性值*/) const;

    //添加子属性
    void addAttribute(const XAttribute&);

    //添加子元素节点
    void addChildElement(const XElement&);
};

//定义ElementList 为 list<XElement>
typedef std::list<XElement> ElementList;


#endif // XELEMENT_H




