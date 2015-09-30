#include "tablecell.h"
#include <Net/NetComponents>
QTableCell::QTableCell()
{
    m_ParamStr = "";
    m_ShowType = "";
    m_TextColor.setBlue(255);
    m_TextAlignment  = Qt::AlignCenter;
    m_TextHAlignment  = Qt::AlignHCenter;
    m_TextVAlignment  = Qt::AlignVCenter;
    m_BackgroundColor.setRgb(255,255,255,0);
    m_TextType = 0;
    m_Text = "";
    m_Width = 100;
    m_Height = 30;
    m_bTopEdgeVisible = true;
    m_bLeftEdgeVisible = true;
}

void QTableCell::SetParamStr(const QString str)
{
	m_ParamStr = str;
}

void QTableCell::SetShowType(const QString str)
{
    m_ShowType = str;
}
//文本颜色
void QTableCell::SetTextColor(const QColor color)
{
	m_TextColor = color;
}
//文本字体
void QTableCell::SetTextFont(const QFont font)
{
	m_TextFont = font;
}
	//水平对齐方式
void QTableCell::SetTextHAlignment(const Qt::Alignment alignment)
{
	m_TextHAlignment = alignment;
}
	//垂直对齐方式
void QTableCell::SetTextVAlignment(const Qt::Alignment alignment)
{
	m_TextVAlignment = alignment;
}

Qt::Alignment QTableCell::GetTextAlignment()
{
	return m_TextVAlignment|m_TextHAlignment;
}
	//背景色
void QTableCell::SetBackgroundColor(const QColor color)
{
	m_BackgroundColor = color;
}


	//文本类型
void QTableCell::SetTextType(const qint8 type)
{	
	m_TextType = type;
}
	//文本内容
void QTableCell::SetText(const QString text)
{
	m_Text = text;
}

void QTableCell::SetWidth(const int w)
{
	m_Width = w;
}
void QTableCell::SetHeight(const int h)
{
	m_Height = h;
}

QString QTableCell::GetTextTypeStr()
{
    QString str = "";
    switch (m_TextType) {
	case tp_Default:
        str = m_ParamStr + "(默认)";
		break;
	case tp_No:
        str = m_ParamStr + "(编号)";
		break;
	case tp_Name:
        str = m_ParamStr + "(名称)";
		break;
    case tp_Mark:
        str = m_ParamStr + "(代号)";
		break;
    case tp_Dimension:
        str = m_ParamStr + "(量纲)";
        break;
    case tp_Unit:
        str = m_ParamStr + "(dw)";
		break;
	case tp_UpperLimite:
        str = m_ParamStr + "(上限)";
		break;
	case tp_LowerLimite:
        str = m_ParamStr + "(下限)";
		break;
	case tp_DisplayType:
        str = m_ParamStr + "(显示格式)";
		break;
	case tp_TheoryValue:
        str = m_ParamStr + "(理论值)";
		break;
	case tp_TheoryValueRange:
        str = m_ParamStr + "(范围)";
		break;
	case tp_TransType:
        str = m_ParamStr + "(传输格式)";
		break;
	case tp_ConvType:
        str = m_ParamStr + "(转换格式)";
		break;
    case tp_Remark:
        str = m_ParamStr + "(备注)";
		break;
    case tp_NULL:
        str = m_ParamStr;
        break;
    default:
        str = m_ParamStr;
		break;
    }
	return str;
}

QString QTableCell::GetConfigModeContent()
{
    if(m_Text.length() > 0)
        return m_Text;
    else
        return GetTextTypeStr();
}

void QTableCell::SetTopEdgeVisible(const bool b)
{
    m_bTopEdgeVisible = b;
}

void QTableCell::SetLeftEdgeVisible(const bool b)
{
    m_bLeftEdgeVisible = b;
}
