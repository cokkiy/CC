#ifndef PARAMSTYLECONFIG_H
#define PARAMSTYLECONFIG_H
#include "paramstyle.h"
#include "paramindex.h"
#include <QString>

class QParamStyleConfig
{
private:
    //样式索引链表
    list<QParamIndex> m_ParamIndexList;
    //样式链表
    list<QParamStyle> m_ParamStyleList;
public:
    QParamStyleConfig();
    bool LoadParamFromXML(const QString& dir);
    //获取样式索引链表
    list<QParamIndex>* getParamIndexList();
    //样式链表
    list<QParamStyle>* getParamStyleList();
};
#endif // PARAMSTYLECONFIG_H
