#pragma once
#include <list>
#include "CompareOperation.h"
#include <QString>
#include <QObject>
/**********************************************************
 @class FilterOperation：定义了过滤操作,过滤操作包含多个
 比较操作\see CompareOperation
 **********************************************************/
class FilterOperations
{
public:
    //关系操作符
    enum RelationOperator
    {
        And,/*并且*/
        Or,/*或者*/
    };
    FilterOperations();
    ~FilterOperations();

    /*!
    检查是否有比较操作符
    @return bool
    作者：cokkiy（张立民)
    创建时间：2015/11/04 21:48:51
    */
    bool empty() const;
    /*!
    加入一个比较操作符的引用
    @param const CompareOperation& operation 要加入的比较操作符
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/04 21:50:34
    */
    void push(const CompareOperation& operation);
    /*!
    加入一个比较操作符和该比较操作符与其他操作符的连接关系
    @param RelationOperator realtion 该比较操作符与其他操作符的连接关系
    @param const CompareOperation& operation 要加入的比较操作符
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/04 21:50:54
    */
    void push(RelationOperator realtion, const CompareOperation& operation);
    /*!
    输出过滤操作的字符串表达形式
    @return QString 过滤操作的字符串表达形式, \see parse
    作者：cokkiy（张立民)
    创建时间：2015/11/04 21:59:10
    */
    QString toString();

    /*!
    从字符串产生过滤操作表达式
    @param const QString & filterString  字符串形式的过滤操作
    @return bool 分析是否正确完成
    作者：cokkiy（张立民)
    创建时间：2015/11/04 22:01:01
    */
    bool parse(const QString& filterString);

    /*!
    清空所有比较操作
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/05 22:47:50
    */
    void clear();
private:
    //比较操作符栈
    std::list<CompareOperation> operations;
    //关系操作符栈
    std::list<RelationOperator> relations;

    //把关系操作符转换为字符串
    QString relationToString(RelationOperator relation)
    {
        if (relation == And)
        {
            return QObject::tr("And");
        }
        else
        {
            return QObject::tr("Or");
        }
    }

    //按关系操作符分割为多个比较操作
    QStringList splitByRelationOperator(const QString& filterString);
};

