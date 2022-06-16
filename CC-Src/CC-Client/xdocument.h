// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：xdocument.h
// 文件摘要：定义了XDocument类，定义了加载xml文档方法和
//          查找子元素的各种方法
// 使用说明：用Load方法之一加载xml配置文件，加载后会自动分析处理，如果返回true说明
//          分析处理成功。
//          用getChild()方法获取根元素XElement，然后用XElement提供的获取其他子元素的
//          方法获取对应的子元素，所有子元素均是XElement，所以可以嵌套向下获取，直到所需
//          的元素位置
//
// 原始版本：1.0
// 作   者：张立民
// 完成日期：2015-08-26
//**************************************************************//
#ifndef XDOCUMENT_H
#define XDOCUMENT_H
#include "xelement.h"
#include "xattribute.h"
#include <QFile>
#include <stack>
#include <QString>

class XDocument
{
public:
    XDocument();

    //获取xml文档的第一级子节点
    XElement getChild() const;

    //从指定路径的文件中加载xml文档
    bool Load(QString /*完整的文件路径*/);

    // 从输入设备中加载xml文档
    bool Load(QIODevice*);

private:

    //子节点
    XElement m_child;

    //分析xml数据原始，构建xml节点
    bool parse(const QString& /*xml文档内容*/);

    //一个标签结束，根据标签类型进行处理
    bool finishToken(int /*标签类型*/, stack<XAttribute>& /*未处理完的属性列表*/,
                     stack<XElement>& /*未处理完的元素列表*/, const QString& /*当前标签*/);

    //结束一个元素
    void finishElement(stack<XElement>& /*未处理完的元素列表*/);

    //行计数
    int line = 1;
};

#endif // XDOCUMENT_H
