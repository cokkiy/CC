#include "tablecell.h"
#include <Net/NetComponents>
QTableCell::QTableCell()
{
    m_ParamStr = QObject::tr("");
    m_ShowType = QObject::tr("");
    m_FormulaStr = QObject::tr("");
	m_Text = QObject::tr("");
    m_TextColor.setBlue(255);
    m_TextAlignment  = Qt::AlignCenter;
    m_TextHAlignment  = Qt::AlignHCenter;
    m_TextVAlignment  = Qt::AlignVCenter;
    m_BackgroundColor.setRgb(255,255,255,0);
    m_TextType = param_Value;
    m_Width = 100;
    m_Height = 30;
    m_bTopEdgeVisible = true;
    m_bLeftEdgeVisible = true;
    m_TextFont.setPointSize(14);
    //左边距
    m_LeftMargin = 1;
    //右边距
    m_RightMargin = 1;
    //上边距
    m_TopMargin = 1;
    //下边距
    m_BottomMargin = 1;
}
//关联参数字符串，包含了参数名
void QTableCell::setParamStr(const QString str)
{
	m_ParamStr = str;
}
//公式字符串
void QTableCell::setFormulaStr(const QString str)
{
    m_FormulaStr = str;
}
//参数显示格式
void QTableCell::setShowType(const QString str)
{
    m_ShowType = str;
}
//文本颜色
void QTableCell::setTextColor(const QColor color)
{
	m_TextColor = color;
}
//文本字体
void QTableCell::setTextFont(const QFont font)
{
	m_TextFont = font;
}
//水平对齐方式
void QTableCell::setTextHAlignment(const Qt::Alignment alignment)
{
	m_TextHAlignment = alignment;
}
//垂直对齐方式
void QTableCell::setTextVAlignment(const Qt::Alignment alignment)
{
	m_TextVAlignment = alignment;
}
//背景色
void QTableCell::setBackgroundColor(const QColor color)
{
	m_BackgroundColor = color;
}
//文本类型
void QTableCell::setTextType(const qint8 type)
{	
	m_TextType = type;
}
	//文本内容
void QTableCell::setText(const QString text)
{
	m_Text = text;
}
//单元格宽度
void QTableCell::setWidth(const int w)
{
	m_Width = w;
}
//单元格高度
void QTableCell::setHeight(const int h)
{
	m_Height = h;
}
//上边框可见
void QTableCell::setTopEdgeVisible(const bool b)
{
    m_bTopEdgeVisible = b;
}
//左边框可见
void QTableCell::setLeftEdgeVisible(const bool b)
{
    m_bLeftEdgeVisible = b;
}
//左边距
void QTableCell::setLeftMargin(const int v)
{
	m_LeftMargin = v;
}
//右边距
void QTableCell::setRightMargin(const int v)
{
	m_RightMargin = v;
}
//上边距
void QTableCell::setTopMargin(const int v)
{
	m_TopMargin = v;
}
//下边距
void QTableCell::setBottomMargin(const int v)
{
	m_BottomMargin = v;
}
//根据单元格的内容形式获取配置模式下单元格的显示字符串
QString QTableCell::getTextTypeStr()
{
    QString str = QObject::tr("");
    QString paramStr = m_ParamStr;
    int pos = paramStr.indexOf(']');
    if(pos != -1)
        paramStr = paramStr.left(pos + 1);
    switch (m_TextType) {
    case param_Value:
        str = paramStr + "(值)";
		break;
    case param_No:
        str = paramStr + "(编号)";
		break;
    case param_Name:
        str = paramStr + "(名称)";
		break;
    case param_Code:
        str = paramStr + "(代号)";
		break;
    case param_Dimension:
        str = paramStr + "(量纲)";
        break;
    case param_Unit:
        str = paramStr + "(单位)";
		break;
    case param_UpperLimite:
        str = paramStr + "(上限)";
		break;
    case param_LowerLimite:
        str = paramStr + "(下限)";
		break;
    case param_DisplayType:
        str = paramStr + "(显示格式)";
		break;
    case param_TheoryValue:
        str = paramStr + "(理论值)";
		break;
    case param_TheoryValueRange:
        str = paramStr + "(范围)";
		break;
    case param_TransType:
        str = paramStr + "(传输格式)";
		break;
    case param_ConvType:
        str = paramStr + "(转换格式)";
		break;
    case param_Remark:
        str = paramStr + "(备注)";
		break;
    case param_NULL:
        str = paramStr + "(null)";
        break;
    default:
        str = paramStr;
		break;
    }
	return str;
}
//获取配置模式下表格的显示内容
QString QTableCell::getConfigModeContent()
{
    if(m_Text.length() > 0)
        return m_Text;
    else
        return getTextTypeStr();
}
//获取文本对齐方式
Qt::Alignment QTableCell::getTextAlignment()
{
	return m_TextVAlignment|m_TextHAlignment;
}
