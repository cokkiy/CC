// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：xattribute.cpp
// 文件摘要：XAttribute类实现，实现了获取xml属性和值的方法
//
// 原始版本：1.0
// 作   者：张立民
// 完成日期：2015-08-26
//**************************************************************//

#include "xattribute.h"

XAttribute::XAttribute()
{
    Name="";
    Value="";
}


XAttribute::XAttribute(QString name, QString value)
{
    Name=name;
    value=value;
}


XAttribute::XAttribute(const XAttribute &xProperty)
{
    this->Name=xProperty.Name;
    this->Value=xProperty.Value;
}
