#ifndef TABLECELL_H
#define TABLECELL_H
#include <QWidget>
#include <QtGui>
#include <QString>
typedef enum TableTextType
{
    tp_Default,
    tp_Name,
    tp_Mark,
    tp_No,
    tp_Dimension,
    tp_UpperLimite,
    tp_LowerLimite,
    tp_DisplayType,
    tp_TheoryValue,
    tp_TheoryValueRange,
    tp_TransType,
    tp_Unit,
    tp_ConvType,
    tp_Remark,
    tp_NULL,
}TableTextType;

class QTableCell
{
public:
    QTableCell();
private:
	//关联参数
	QString m_ParamStr;
	//参数显示格式
    QString m_ShowType;
	//文本颜色
	QColor m_TextColor;
	//文本字体
	QFont m_TextFont;
	//文本对齐方式
	Qt::Alignment m_TextAlignment; 
	//文本水平对齐方式
	Qt::Alignment m_TextHAlignment;
	//文本垂直对齐方式
	Qt::Alignment m_TextVAlignment;
	//单元格背景色
	QColor m_BackgroundColor;
	//内容形式
    qint8 m_TextType;
	//单元格文本
    QString m_Text;
	//单元格高度
    int m_Height;
    //单元格宽度
    int m_Width;
	//上边框显示状态
    bool m_bTopEdgeVisible;
	//左边框显示状态
    bool m_bLeftEdgeVisible;
public:
	QString GetParamStr() const {return m_ParamStr;}
    void SetParamStr(const QString str);
    QString GetShowType() const {return m_ShowType;}
    void SetShowType(const QString str);
    QString GetConfigModeContent();
	QColor GetTextColor() const {return m_TextColor;}
    void SetTextColor(const QColor color);
	QFont GetTextFont() const {return m_TextFont;}
    void SetTextFont(const QFont font);
	Qt::Alignment GetTextAlignment();
	Qt::Alignment GetTextHAlignment() const {return m_TextHAlignment;}
    void SetTextHAlignment(const Qt::Alignment alignment);
	Qt::Alignment GetTextVAlignment() const {return m_TextVAlignment;}
    void SetTextVAlignment(const Qt::Alignment alignment);
	QColor GetBackgroundColor() const {return m_BackgroundColor;}
    void SetBackgroundColor(const QColor color);
    QString GetTextTypeStr();
    void SetTextType(const qint8 type);
	QString GetText() const {return m_Text;}
    void SetText(const QString text);
    int GetWidth() const {return m_Width;}
    void SetWidth(const int w);
    int GetHeight() const {return m_Height;}
    void SetHeight(const int h);
    //上边框显示状态
    bool GetTopEdgeVisible() const {return m_bTopEdgeVisible;}
    void SetTopEdgeVisible(const bool b);
    //左边框显示状态
    bool GetLeftEdgeVisible() const {return m_bLeftEdgeVisible;}
    void SetLeftEdgeVisible(const bool b);
};

#endif // TABLECELL_H
