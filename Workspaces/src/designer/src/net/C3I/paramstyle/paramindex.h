#ifndef PARAMINDEX_H
#define PARAMINDEX_H
#include <QString>
#include <QtGui/QFont>

class QParamIndex
{
public:
    QParamIndex();
private:
    //索引名称
    QString m_IndexName;
    //文本
    QString m_Text;
    //颜色
    QString m_ColorStr;
    //字体名称
    QString m_FontNameStr;
    //字体大小
    QString m_FontSizeStr;
    //粗体
    QString m_bFontBoldStr;
    //斜体
    QString m_bFontItalicStr;
public:
    //索引名称
    QString getIndexName() const {return m_IndexName;}
    void setIndexName(const QString str);

    //文本
    QString getText() const {return m_Text;}
    void setText(const QString str);
    //颜色
    void setColorStr(QString str);
    QString getColorStr() const {return m_ColorStr;}

    //字体名称
    QString getFontNameStr() const {return m_FontNameStr;}
    void setFontNameStr(const QString str);
    //字体大小
    QString getFontSizeStr() const {return m_FontSizeStr;}
    void setFontSizeStr(const QString str);
    //粗体
    QString getFontBoldStr() const {return m_bFontBoldStr;}
    void setFontBoldStr(const QString str);
    //斜体
    QString getFontItalicStr() const {return m_bFontItalicStr;}
    void setFontItalicStr(const QString str);
};

#endif // PARAMINDEX_H
