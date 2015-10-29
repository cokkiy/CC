#include "paramindex.h"
QParamIndex::QParamIndex()
{
    m_IndexName = QObject::tr("");
    m_Text = QObject::tr("");
    m_ColorStr = QObject::tr("");
    m_FontNameStr = QObject::tr("");
    //字体大小
    m_FontSizeStr = QObject::tr("");
    //粗体
    m_bFontBoldStr = QObject::tr("");
    //斜体
    m_bFontItalicStr = QObject::tr("");

}
//索引名称
void QParamIndex::setIndexName(const QString str)
{
    m_IndexName = str;
}
//颜色
void QParamIndex::setColorStr(QString str)
{
    str = str.right(str.length() - 1);
    m_ColorStr = str;
}
//文本
void QParamIndex::setText(const QString str)
{
    m_Text = str;
}

void QParamIndex::setFontNameStr(const QString str)
{
    m_FontNameStr = str;
}
//字体大小
void QParamIndex::setFontSizeStr(const QString str)
{
    m_FontSizeStr = str;
}
//粗体
void QParamIndex::setFontBoldStr(const QString str)
{
    m_bFontBoldStr = str;
}
//斜体
void QParamIndex::setFontItalicStr(const QString str)
{
    m_bFontItalicStr = str;
}
