#include "FilterOperations.h"
#include <QRegularExpression>

FilterOperations::FilterOperations()
{
}


FilterOperations::~FilterOperations()
{
}

/*!
检查是否有比较操作符
@return bool
作者：cokkiy（张立民)
创建时间：2015/11/04 21:48:51
*/
bool FilterOperations::empty() const
{
    return operations.empty();
}

/*!
清空所有比较操作
@return void
作者：cokkiy（张立民)
创建时间：2015/11/05 22:47:50
*/
void FilterOperations::clear()
{
    operations.clear();
    relations.clear();
}

/*!
加入一个比较操作符的引用
@param const CompareOperation& operation 要加入的比较操作符
@return void
作者：cokkiy（张立民)
创建时间：2015/11/04 21:50:34
*/
void FilterOperations::push(const CompareOperation& operation)
{
    if (empty())
    {
        operations.push_front(operation);
    }
    else
    {
        operations.push_front(operation);
        //默认用And关系符
        relations.push_front(And);
    }
}

/*!
加入一个比较操作符和该比较操作符与其他操作符的连接关系
@param RelationOperator realtion 该比较操作符与其他操作符的连接关系
@param const CompareOperation& operation 要加入的比较操作符
@return void
作者：cokkiy（张立民)
创建时间：2015/11/04 21:50:54
*/
void FilterOperations::push(RelationOperator realtion, const CompareOperation& operation)
{
    if (!empty())
    {
        //存入关系操作符
        relations.push_front(realtion);
    }
    //存入比较操作符
    operations.push_front(operation);
}

/*!
输出过滤操作的字符串表达形式
@return QString 过滤操作的字符串表达形式, \see parse
作者：cokkiy（张立民)
创建时间：2015/11/04 21:59:10
*/
QString FilterOperations::toString()
{
    QString filter = QString("");
    auto operationIter = operations.rbegin();
    auto relationIter = relations.rbegin();
    for (; operationIter != operations.rend(); operationIter++)
    {
        filter += operationIter->toString();
        if (relationIter != relations.rend())
        {
            filter += QString(" %1 ").arg(relationToString(*relationIter));
            relationIter++;
        }
    }

    return filter;
}

/*!
从字符串产生过滤操作表达式
@param const QString & filterString  字符串形式的过滤操作
@return bool 分析是否正确完成
作者：cokkiy（张立民)
创建时间：2015/11/04 22:01:01
*/
bool FilterOperations::parse(const QString& filterString)
{
    QStringList compares = splitByRelationOperator(filterString);
    for (auto compare : compares)
    {
        CompareOperation co;
        co.parse(compare);
        this->operations.push_front(co);
        int index = -1;
        index = filterString.indexOf(QRegularExpression(QObject::tr("And") + "|" + QObject::tr("Or")), index);
        if (index != -1)
        {
            QString r = filterString.mid(index);
            if (r.startsWith(QObject::tr("And")))
            {
                this->relations.push_front(And);
            }
            else
            {
                this->relations.push_front(Or);
            }
        }
    }
    return true;
}

//按关系操作符分割为多个比较操作
QStringList FilterOperations::splitByRelationOperator(const QString& filterString)
{
    return filterString.split(QRegularExpression(QObject::tr("And")+"|"+QObject::tr("Or")), QString::SkipEmptyParts);
}
