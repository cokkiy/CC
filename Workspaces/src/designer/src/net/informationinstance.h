#ifndef INFORMATIONINSTANCE_H
#define INFORMATIONINSTANCE_H
#include "C3I/paramstyle/paramstyleconfig.h"
#include "abstractinformation.h"

class CParamInfoRT;

class NETSHARED_EXPORT InformationInstance : public InformationInterface
{
    Q_OBJECT
public:
    explicit InformationInstance(QObject *parent = 0);
    /*!
      装载某路径下的所有xml表,
      包含"zx_param_global_"字符串的信息约定表xml文件,
      包含"table_info_global_"字符串的表名约定表xml文件
      @param QString dir  文件路径名
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int load(QString dir);
    /*!
      获取所有表的表名
      @return TabList  所有表的表名列表
    */
    virtual TabMap getTabList();
    /*!
      获取所有参数的映射
      @return ParamMap  所有参数的映射
    */
    virtual ParamMap getParamMap();

    
   
    
    /*!
    根据表号获取单个表的配置文件
    @param unsigned short tabNo 
    @return SingleTableParamVector
    作者：cokkiy（张立民)
    创建时间：2015/09/25 18:03:37
    */
    virtual SingleTableParamVector getParamVector(unsigned short tabNo);

    /*!
      通过表号和参数号获取某参数
      @param unsigned short tabNo  表号
      @param unsigned short paramNo  参数号
      @return AbstractParam*  某参数指针,NULL表示为空
    */
    virtual AbstractParam* getParam(unsigned short tabNo,unsigned short paramNo);
    /*!
      通过数据字符串获取某参数指针
      @param QString data  数据字符串
      @return AbstractParam*  某参数指针,NULL表示为空
    */
    virtual AbstractParam* getParam(QString data);

    //样式链表
    virtual list<QParamStyle>* getParamStyleList();
private:
    TabMap  m_tabMap;
    ParamMap m_paramMap;
};

#endif // INFORMATIONINSTANCE_H
