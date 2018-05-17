#include "CompareOperation.h"
#include <QObject>
/*!
创建一个比较操作CompareOperation的实例
@param QString property 要进行比较操作的属性
@param QString operation 比较操作符号
@return \class CompareOperation的实例
作者：cokkiy（张立民)
创建时间：2015/11/05 11:13:34
*/
CompareOperation::CompareOperation(Property property, Operator operation)
{
    this->m_Property = property;
    this->m_Operator = operation;
}

/*!
复制构造函数
@param const CompareOperation & ref 复制的对象
@return \class CompareOperation的实例
作者：cokkiy（张立民)
创建时间：2015/11/05 11:15:49
*/
CompareOperation::CompareOperation(const CompareOperation& ref)
{
    this->m_Property = ref.m_Property;
    this->m_Operator = ref.m_Operator;
    this->Value1 = ref.Value1;
    this->Value2 = ref.Value2;
}

CompareOperation::~CompareOperation()
{
}

/*!
形成表示比较操作的字符串形式表达式
@return QString 比较操作的字符串形式表达式
作者：cokkiy（张立民)
创建时间：2015/11/05 9:44:21
*/
QString CompareOperation::toString()
{
    return property2String() + " " + operator2String();
}

//比较操作转换成字符串
QString CompareOperation::operator2String()
{
    QString result = QStringLiteral("");
    switch (m_Operator)
    {
    case CompareOperation::Equals:
        result = QStringLiteral("= %1").arg(Value1);
        break;
    case CompareOperation::NotEquals:
        result = QStringLiteral("!= %1").arg(Value1);
        break;
    case CompareOperation::GreaterThan:
        result = QStringLiteral(">= %1").arg(Value1);
        break;
    case CompareOperation::LessThan:
        result = QStringLiteral("<= %1").arg(Value1);
        break;
    case CompareOperation::Between:
        result = QStringLiteral("介于 %1 和 %2 之间").arg(Value1).arg(Value2);
        break;
    case CompareOperation::Same:
        result = QStringLiteral("= %1").arg(Value1);
        break;
    case CompareOperation::Startwiths:
        result = QStringLiteral("以\"%1\"开始").arg(Value1);
        break;
    case CompareOperation::Endwiths:
        result = QStringLiteral("以\"%1\"结束").arg(Value1);
        break;
    case CompareOperation::Contains:
        result = QStringLiteral("包含\"%1\"").arg(Value1);
        break;
    default:
        result = QStringLiteral("无法识别的比较操作");
        break;
    }

    return result;
}

//查找操作符
CompareOperation::Operator CompareOperation::findOperator(QString& compare)
{
    int index = -1;
    Operator op = Equals;
    if ((index = compare.indexOf("=")) != -1)
    {
        op = Equals;
    }
    else if ((index = compare.indexOf("!=")) != -1)
    {
        op = NotEquals;
    }
    else if ((index = compare.indexOf("<=")) != -1)
    {
        op = LessThan;
    }
    else if ((index = compare.indexOf(">=")) != -1)
    {
        op = GreaterThan;
    }
    else if ((index = compare.indexOf(QObject::tr("Between"))) != -1)
    {
        op = Between;
    }
    else if ((index = compare.indexOf(QObject::tr("Startwiths"))) != -1)
    {
        op = Startwiths;
    }
    else if ((index = compare.indexOf(QObject::tr("Endwiths"))) != -1)
    {
        op = Endwiths;
    }
    else if ((index = compare.indexOf(QObject::tr("Same"))) != -1)
    {
        op = Same;
    }

    if (index != -1)
    {
        compare = compare.mid(index);
    }
    return op;
}

//要比较的属性转换成字符串
QString CompareOperation::property2String()
{
    QString result = QStringLiteral("");
    switch (m_Property)
    {
    case CompareOperation::IP:
        result= QStringLiteral("IP");
        break;
    case CompareOperation::State:
        result = QStringLiteral("状态");
        break;
    case CompareOperation::Name:
        result = QStringLiteral("名称");
        break;
    default:
        result = QStringLiteral("未知属性");
        break;
    }
    return result;
}

/*!
设置值1
@param QString v1  值1的字符串表达式
@return void
作者：cokkiy（张立民)
创建时间：2015/11/05 11:35:25
*/
void CompareOperation::setValue1(QString v1)
{
    Value1 = v1;
}

/*!
设置值2
@param QString v2  值2的字符串表达式
@return void
作者：cokkiy（张立民)
创建时间：2015/11/05 11:35:25
*/
void CompareOperation::setValue2(QString v2)
{
    Value2 = v2;
}

/*!
分析字符串,产生比较表达式
@param QString compare 要分析的字符串
@return void
作者：cokkiy（张立民)
创建时间：2015/11/05 21:34:05
*/
void CompareOperation::parse(QString compare)
{
    int pos = compare.indexOf(QStringLiteral(" ")); //寻找第一个空格
    if (pos != -1)
    {
        QString str_property = compare.left(pos);
        if (str_property.compare(QObject::tr("IP"), Qt::CaseInsensitive) == 0)
        {
            this->m_Property = IP;
        }
        else if(str_property.compare(QObject::tr("State"), Qt::CaseInsensitive) == 0)
        {
            this->m_Property = State;
        }
        else
        {
            this->m_Property = Name;
        }
        compare = compare.mid(pos);
        this->m_Operator = findOperator(compare);
        this->Value1 = findValue1(compare);
        this->Value2 = findValue2(compare);
    }
}

//从字符串中找值2
QString CompareOperation::findValue2(const QString& compare)
{
    if (m_Operator == Between)
    {
        //介于 %1 和 %2 之间
        int pos0 = compare.indexOf(QStringLiteral("介于"));
        int pos1 = compare.indexOf(QStringLiteral("和"));
        int pos2= compare.indexOf(QStringLiteral("之间"));
        return compare.mid(pos1, pos2);
    }
    return QStringLiteral("");
}

//从字符串中找值1
QString CompareOperation::findValue1(const QString& compare)
{
    QString v1 = QStringLiteral("");
    switch (m_Operator)
    {
    case CompareOperation::Equals:
    {
        int pos = compare.indexOf("=");
        if (pos != -1)
        {
            v1 = compare.mid(pos).trimmed();
        }
    }
        break;
    case CompareOperation::GreaterThan:
    {
        int pos = compare.indexOf(">=");
        v1 = compare.mid(pos).trimmed();
    }
        break;
    case CompareOperation::LessThan:
    {
        int pos = compare.indexOf("<=");
        v1 = compare.mid(pos).trimmed();
    }
        break;
    case CompareOperation::Between:
    {
        int pos0 = compare.indexOf(QStringLiteral("介于"));
        int pos1 = compare.indexOf(QStringLiteral("和"));
        v1 = compare.mid(pos0, pos1).trimmed();
    }
        break;
    case CompareOperation::NotEquals:
    {
        int pos = compare.indexOf("!=");
        v1 = compare.mid(pos).trimmed();
    }
        break;
    case CompareOperation::Same:
    {

    }
        break;
    case CompareOperation::Startwiths:
    {
        //以\"%1\"开始
        int pos0 = compare.indexOf(QStringLiteral("以\""));
        int pos1 = compare.indexOf(QStringLiteral("\"开始"));
        v1 = compare.mid(pos0, pos1).trimmed();
    }
        break;
    case CompareOperation::Endwiths:
    {
        //以\"%1\"结束
        int pos0 = compare.indexOf(QStringLiteral("以\""));
        int pos1 = compare.indexOf(QStringLiteral("\"结束"));
        v1 = compare.mid(pos0, pos1).trimmed();
    }
        break;
    case CompareOperation::Contains:
    {
        //包含\"%1\"
        int pos0 = compare.indexOf(QStringLiteral("\""));
        int pos1 = compare.indexOf(QStringLiteral("\""), pos0);
        v1 = compare.mid(pos0, pos1).trimmed();
    }
        break;
    default:
        break;
    }

    return v1;
}
