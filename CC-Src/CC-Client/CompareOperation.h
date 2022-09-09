#pragma once
#include <QString>
/****************************************************************
 @class CompareOperation:过滤比较操作类,定义了过滤
 比较操作具有的属性
 ****************************************************************/
class CompareOperation
{
public:
    //要进行比较操作的属性
    enum Property
    {
        IP=0, /*IP*/
        State,/*状态*/
        Name,/*名称*/
    };
    //比较符号（类型)
    enum Operator {
        Equals = 0,/*等于*/        
        GreaterThan, /*大于*/
        LessThan,/*小于*/
        Between,/*介于*/
        NotEquals, /*不等于*/
        Same=10,/*字符串比较,相同*/
        Startwiths=11,/*字符串比较,以字符开始*/
        Endwiths,/*字符串比较,以字符结束*/
        Contains,/*包含字符*/
    }; 

    /*!
    创建一个比较操作CompareOperation的实例
    @param Property property 要进行比较操作的属性
    @param Operator operation 比较操作符号
    @return \class CompareOperation的实例
    作者：cokkiy（张立民)
    创建时间：2015/11/05 11:13:34
    */
    CompareOperation(Property property, Operator operation);
    /*!
    复制构造函数
    @param const CompareOperation & ref 复制的对象
    @return \class CompareOperation的实例
    作者：cokkiy（张立民)
    创建时间：2015/11/05 11:15:49
    */
    CompareOperation(const CompareOperation& ref);

    //默认构造函数,创建一个空对象
    CompareOperation() {};

    ~CompareOperation();

    /*!
    形成表示比较操作的字符串形式表达式
    @return QString 比较操作的字符串形式表达式
    作者：cokkiy（张立民)
    创建时间：2015/11/05 9:44:21
    */
    QString toString();
    
    /*!
    设置值1
    @param QString v1  值1的字符串表达式
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/05 11:35:25
    */
    void setValue1(QString v1);

    /*!
    设置值2
    @param QString v2  值2的字符串表达式
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/05 11:35:32
    */
    void setValue2(QString v2);

    /*!
    分析字符串,产生比较表达式
    @param QString compare 要分析的字符串
    @return void
    @remark \see toString()
    作者：cokkiy（张立民)
    创建时间：2015/11/05 21:34:05
    */
    void parse(QString compare);

private:
    //要比较的属性
    Property m_Property;
    //比较操作符
    Operator m_Operator;
    //值1
    QString Value1;
    //值2
    QString Value2;

    //属性转成字符串
    QString property2String();

    //操作转成字符串
    QString operator2String();  

    //查找操作符
    Operator findOperator(QString& compare);

    //从字符串中找值2
    QString findValue2(const QString& compare);
    
    //从字符串中找值1
    QString findValue1(const QString& compare);
};

