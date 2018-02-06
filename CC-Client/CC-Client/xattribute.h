// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：xproperty.h
// 文件摘要：XProperty类定义，定义了xml属性结构
//
// 原始版本：1.0
// 作   者：张立民
// 完成日期：2015-08-26
//**************************************************************//

#ifndef XATTRIBUTE_H
#define XATTRIBUTE_H

#include <QString>

//XML属性
class XAttribute
{
public:
    //属性名称
    QString Name;

    //属性值
    QString Value;

    //构造函数
    XAttribute();
    XAttribute(QString name,QString value);
    XAttribute(const XAttribute&);
};

#endif // XATTRIBUTE_H
