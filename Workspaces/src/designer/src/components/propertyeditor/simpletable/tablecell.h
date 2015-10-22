#ifndef TABLECELL_H
#define TABLECELL_H
#include <QWidget>
#include <QtGui>
#include <QString>

typedef enum TableTextType
{
    param_Value,
    param_Name,
    param_Code,
    param_No,
    param_Dimension,
    param_UpperLimite,
    param_LowerLimite,
    param_DisplayType,
    param_TheoryValue,
    param_TheoryValueRange,
    param_TransType,
    param_Unit,
    param_ConvType,
    param_Remark,
    param_NULL,
}TableTextType;

class QTableCell
{
public:
    QTableCell();
private:
	//关联参数字符串，包含了参数名
	QString m_ParamStr;
    //公式字符串
    QString m_FormulaStr;
	//单元格文本
    QString m_Text;
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
	//单元格高度
    int m_Height;
    //单元格宽度
    int m_Width;
	//上边框可见
    bool m_bTopEdgeVisible;
	//左边框可见
    bool m_bLeftEdgeVisible;
	//左边距
	int m_LeftMargin;
	//右边距
	int m_RightMargin;
	//上边距
	int m_TopMargin;
	//下边距
	int m_BottomMargin;
public:
	//关联参数字符串，包含了参数名
    QString getParamStr() const {return m_ParamStr;}
    void setParamStr(const QString str);
	//公式字符串
    QString getFormulaStr() const {return m_FormulaStr;}
    void setFormulaStr(const QString str);
	//参数显示格式
    QString getShowType() const {return m_ShowType;}
    void setShowType(const QString str);
	//文本颜色
    QColor getTextColor() const {return m_TextColor;}
    void setTextColor(const QColor color);
	//文本字体
    QFont getTextFont() const {return m_TextFont;}
    void setTextFont(const QFont font);
	//文本水平对齐方式
    Qt::Alignment getTextHAlignment() const {return m_TextHAlignment;}
    void setTextHAlignment(const Qt::Alignment alignment);
	//文本垂直对齐方式
    Qt::Alignment getTextVAlignment() const {return m_TextVAlignment;}
    void setTextVAlignment(const Qt::Alignment alignment);
	//单元格背景色
    QColor getBackgroundColor() const {return m_BackgroundColor;}
    void setBackgroundColor(const QColor color);
	//内容形式
    qint8 getTextType() const {return m_TextType;}
    void setTextType(const qint8 type);
	//单元格文本
    QString getText() const {return m_Text;}
    void setText(const QString text);
	//单元格宽度
    int getWidth() const {return m_Width;}
    void setWidth(const int w);
	//单元格高度
    int getHeight() const {return m_Height;}
    void setHeight(const int h);
    //上边框可见
    bool getTopEdgeVisible() const {return m_bTopEdgeVisible;}
    void setTopEdgeVisible(const bool b);
    //左边框可见
    bool getLeftEdgeVisible() const {return m_bLeftEdgeVisible;}
    void setLeftEdgeVisible(const bool b);
	//左边距
    int getLeftMargin() const {return m_LeftMargin;}
    void setLeftMargin(const int v);
	//右边距
    int getRightMargin() const {return m_RightMargin;}
    void setRightMargin(const int v);
	//上边距
    int getTopMargin() const {return m_TopMargin;}
    void setTopMargin(const int v);
	//下边距
    int getBottomMargin() const {return m_BottomMargin;}
    void setBottomMargin(const int v);
	//根据单元格的内容形式获取配置模式下单元格的显示字符串
	QString getTextTypeStr();
	//获取文本对齐方式
	Qt::Alignment getTextAlignment();
	//获取配置模式下表格的显示内容
    QString getConfigModeContent();
};

#endif // TABLECELL_H
