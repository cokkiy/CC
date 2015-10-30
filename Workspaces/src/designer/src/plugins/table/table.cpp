#include "table.h"
#include <QDebug>
QTable::QTable(QWidget *parent) :
    QWidget(parent)
{
    //文本属性
    m_textString = tr("");
    m_lastTextString = tr("");
    dispString = tr("");
    zebraLineColor.setRgb(255,255,255,127);

    //网格线属性
    lineColor.setRgb(0,0,0);
    lineWidth = 1;
	lineStyle = Qt::SolidLine;

    //插件框架属性
    QColor Color;
    Color.setAlpha(255);
    Color.setRgb(0);
    backgroundColor.setColor(Color);
    borderColor.setRgb(0,0,0);
    borderWidth = 1;
    borderStyle = Qt::SolidLine;
    pluginRect.setRect(0,0,0,0);
    m_timer_id = startTimer(500);
	m_fWidthRatio = 1.0;
    m_fHeightRatio = 1.0;
	dc = NetComponents::getDataCenter();
    paramInformation = NetComponents::getInforCenter();
}
QTable::~QTable()
{
    killTimer(m_timer_id);
    if(dc)
    {
        delete dc;
        dc = NULL;
    }
}
//绘制事件
void QTable::paintEvent(QPaintEvent *)
{
    showPlugin();
}
//改变图元大小事件
void QTable::resizeEvent(QResizeEvent *event)
{
	if(m_TableProperty.getRowNum() == 0 || m_TableProperty.getColNum() == 0)
        return;
    QSize newSize = event->size();
    if(m_TableProperty.getTableVector()->count() != 0 )
    {
        QSize tableSize = m_TableProperty.size();
        m_fWidthRatio = ((float)newSize.width())/(float)tableSize.width();
        m_fHeightRatio = ((float)newSize.height())/(float)tableSize.height();
        m_textString = m_TableProperty.toJsonStr(m_fWidthRatio,m_fHeightRatio);
        m_lastTextString = m_textString;
    }
}
//文本属性
void QTable::setTextString(const QString string)
{
    m_textString = string;
    update();
    updateGeometry();
}
//显示插件边框
void QTable::showPluginBord()
{
    QPainter painter(this);
    QPen pen = setCustomPen(borderStyle,borderColor,borderWidth);
    painter.setPen(pen);
    painter.drawRect(pluginRect);
}
//显示插件背景
void QTable::showPluginBackgroud()
{
    setPluginRect();
    QPainter painter(this);
    painter.fillRect(pluginRect,backgroundColor);
}
//显示表格文本
void QTable::showTableText(float wr,float hr)
{
    QPainter painter(this);
    int width = 0;
    int height = 0;
    int lastheight = 0;
    for(int i = 0; i < m_TableProperty.getRowNum(); i++)
    {
        QTableCell TableCell = m_TableProperty.getTableVector()->at(i*m_TableProperty.getColNum());
        height += TableCell.getHeight()*hr;
        width = 0;
        int lastwidth = 0;
        for(int j = 0;j< m_TableProperty.getColNum(); j++)
        {
            TableCell = m_TableProperty.getTableVector()->at(i*m_TableProperty.getColNum() + j);
            width += TableCell.getWidth()*wr;
            QRect rect(lastwidth + TableCell.getLeftMargin(),lastheight + TableCell.getTopMargin() ,
                                   width - lastwidth - TableCell.getRightMargin(),height - lastheight - TableCell.getBottomMargin());
            painter.fillRect(rect,TableCell.getBackgroundColor());
            dispString = getCellShowStr(TableCell);
            QFont styleFont = TableCell.getTextFont();
            QColor styleColor = TableCell.getTextColor();
            analyzeDisplayStr(styleFont,styleColor);
            painter.setFont(styleFont);
            painter.setPen(styleColor);
            painter.drawText(rect,TableCell.getTextAlignment()|Qt::TextWrapAnywhere,dispString);
            lastwidth = width;
        }
        lastheight = height;
    }
}
//显示表格
void QTable::showTable(float wr,float hr)
{
    QPainter painter(this);
    QPen pen = setCustomPen(lineStyle,lineColor,lineWidth);
    painter.setPen(pen);
    int width = 0;
    int height = 0;
    int lastheight = 0;
    for(int i = 0; i < m_TableProperty.getRowNum(); i++)
    {
        QTableCell TableCell = m_TableProperty.getTableVector()->at(i*m_TableProperty.getColNum());
        height += TableCell.getHeight()*hr;
        width = 0;
        int lastwidth = 0;
        for(int j = 0;j< m_TableProperty.getColNum(); j++)
        {
            TableCell = m_TableProperty.getTableVector()->at(j);
            width += TableCell.getWidth()*wr;
            TableCell =  m_TableProperty.getTableVector()->at(i*m_TableProperty.getColNum() + j);
            if(i != 0)
            {
                if(TableCell.getTopEdgeVisible())
                {
                    QPoint pt1;
                    pt1.setX(lastwidth);
                    pt1.setY(lastheight);
                    QPoint pt2;
                    if(j != (m_TableProperty.getColNum()-1))
                        pt2.setX(width);
                    else
                        pt2.setX(this->geometry().width());
                    pt2.setY(lastheight);
                    painter.drawLine(pt1,pt2);
                }
            }
            if(j != 0)
            {
                if(TableCell.getLeftEdgeVisible())
                {
                    QPoint pt1;
                    pt1.setX(lastwidth);
                    pt1.setY(lastheight);
                    QPoint pt2;
                    pt2.setX(lastwidth);
                    if(i == (m_TableProperty.getRowNum()-1))
                        pt2.setY(this->geometry().height()+1);
                    else
                        pt2.setY(height);
                    painter.drawLine(pt1,pt2);
                }
            }
            lastwidth = width;
        }
        lastheight = height;
    }
}
//设置插件大小和位置
void QTable::setPluginGeometry()
{
    QSize tableSize = m_TableProperty.size();
    setGeometry(this->geometry().x(),this->geometry().y(),tableSize.width(),tableSize.height());
}

void QTable::timerEvent(QTimerEvent *)
{
    update();
}
//显示插件
void QTable::showPlugin()
{
    initTableProperty();
    showPluginBackgroud();
    showZebraLine(m_fWidthRatio,m_fHeightRatio);
    showTableText(m_fWidthRatio,m_fHeightRatio);
    showTable(m_fWidthRatio,m_fHeightRatio);
    showPluginBord();
}
//插件边框颜色
void QTable::setBorderColor(const QColor Color)
{
    borderColor = Color;
    update();
    updateGeometry();
}
//插件边框线样式
void QTable::setBorderStyle(const Qt::PenStyle style)
{
    borderStyle = style;
    update();
    updateGeometry();
}
//插件背景色
void QTable::setBackgroundBrush(const QBrush Brush)
{
    backgroundColor = Brush;
    update();
    updateGeometry();
}
//插件边框线宽
void QTable::setBorderWidth(const qint32 width)
{
    borderWidth = width;
    update();
    updateGeometry();
}
//显示斑马线
void QTable::showZebraLine(float wr,float hr)
{
    QPainter painter(this);
    int lastHeight = 0;
    int height = 0;
    for(int i = 0; i < m_TableProperty.getRowNum(); i++)
    {
        QTableCell TableCell = m_TableProperty.getTableVector()->at(i*m_TableProperty.getColNum());
        height += TableCell.getHeight()*hr;
        int w = m_TableProperty.size().width()*wr;
        int h = 0;
        if(i != m_TableProperty.getRowNum() - 1 )
            h = TableCell.getHeight()*hr;
        else
            h = m_TableProperty.size().height();
        QRect rect(0,lastHeight,w,h);
        if(i%2 == 1)
            painter.fillRect(rect,zebraLineColor);
        lastHeight = height;
    }
}
//设置自定义画笔
QPen QTable::setCustomPen(Qt::PenStyle style,QColor color,uint width)
{
    QPen pen;
    pen.setColor(color);
    pen.setStyle(style);
    pen.setWidth((qint32)width);
    return pen;
}
//设置插件矩形区域和背景区域
void QTable::setPluginRect()
{
    QRect Rect = this->geometry();
    qint16 w = Rect.width();
    qint16 h = Rect.height();
    pluginRect.setRect(borderWidth/2,borderWidth/2,w-borderWidth,h-borderWidth);
}

//斑马线颜色
void QTable::setZebraLineColor(const QColor Color)
{
	zebraLineColor = Color;
	update();
    updateGeometry();
}
//网格线颜色
void QTable::setGridLineColor(const QColor Color)
{
	lineColor = Color;
	update();
    updateGeometry();
}
//网格线宽度
void QTable::setGridLineWidth(const qint32 w)
{
	lineWidth = w;
	update();
    updateGeometry();
}
//网格线样式
void QTable::setGridLineStyle(const Qt::PenStyle style)
{
	lineStyle = style;
	update();
    updateGeometry();
}
//初始化表格属性
void QTable::initTableProperty()
{
    if(m_textString != m_lastTextString || m_textString.isEmpty())
    {
        m_TableProperty.setTableVector(m_textString);
        setPluginGeometry();
        m_lastTextString = m_textString;
    }
}

//获取单元格显示字符串
QString QTable::getCellShowStr(QTableCell TableCell)
{
    QString showStr = TableCell.getText();
    if(!showStr.isEmpty())
        return showStr;
    showStr = tr("");
    QString paramStr = TableCell.getParamStr();
    int pos = paramStr.indexOf(']');
    if(pos != -1)
        paramStr = paramStr.left(pos + 1);
    else
        return showStr;
    if(paramInformation->getParam(paramStr) == NULL)
        return showStr;
    qint8 type = TableCell.getTextType();
    switch (type) {
    case param_Value:
        dc->getString(TableCell.getFormulaStr(),TableCell.getShowType(),showStr);
        break;
    case param_Name:
        showStr = tr(paramInformation->getParam(paramStr)->GetParamName().c_str());
        break;
    case param_Code:
        showStr = tr(paramInformation->getParam(paramStr)->GetParamCode().c_str());
        break;
    case param_No:
        showStr = paramStr;
        break;
    case param_Dimension:
        showStr = QString::number(paramInformation->getParam(paramStr)->GetParamQuotiety());
        break;
    case param_Unit:
        showStr = tr(paramInformation->getParam(paramStr)->GetParamUnit().c_str());
        break;
    case param_UpperLimite:
        showStr = QString::number(paramInformation->getParam(paramStr)->GetParamUprlmt());
        break;
    case param_LowerLimite:
        showStr = QString::number(paramInformation->getParam(paramStr)->GetParamLorlmt());
        break;
    case param_DisplayType:
        showStr = tr(paramInformation->getParam(paramStr)->GetParamShowType().c_str());
        break;
    case param_TheoryValue:
        showStr = tr(paramInformation->getParam(paramStr)->GetParamTheoryValue().c_str());
        break;
    case param_TheoryValueRange:
        showStr = tr(paramInformation->getParam(paramStr)->GetParamTheoryRange().c_str());
        break;
    case param_TransType:
        showStr = tr(AbstractParam::GetTypeString(paramInformation->getParam(paramStr)->GetParamTranType()).c_str());
        break;
    case param_ConvType:
        showStr = tr(AbstractParam::GetTypeString(paramInformation->getParam(paramStr)->GetParamConvType()).c_str());
        break;
    case param_Remark:
        showStr = tr(paramInformation->getParam(paramStr)->GetParamNote().c_str());
        break;
    case param_NULL:
        showStr = tr("");
        break;
    default:
        showStr = tr("");
        break;
    }
    return showStr;
}

//解析显示字符串，字符串中包括字体、颜色、文本
void QTable::analyzeDisplayStr(QFont &styleFont, QColor &styleColor)
{
    QString str = dispString;
    if(str.indexOf("{{") == 0)
    {
        str = str.right(str.length() - 2);
        int pos = str.indexOf("}");
        QString fontStr = str.left(pos);
        str = str.right(str.length() - pos - 1);
        //解析字体
        if(fontStr.length() > 4)
        {
            pos = fontStr.indexOf(",");
            if(pos > 0)
                styleFont.setFamily(fontStr.left(pos));
            fontStr = fontStr.right(fontStr.length() - pos - 1);
            pos = fontStr.indexOf(",");
            if(pos > 0)
                styleFont.setPointSize(fontStr.left(pos).toInt());
            fontStr = fontStr.right(fontStr.length() - pos - 1);
            pos = fontStr.indexOf(",");
            if(pos > 0)
                styleFont.setBold((bool)(fontStr.left(pos).toInt()));
            fontStr = fontStr.right(fontStr.length() - pos - 1);
            if(fontStr.length() > 0)
                styleFont.setItalic((bool)(fontStr.toInt()));
        }
        //解析颜色
        pos = str.indexOf("}");
        QString colorStr = str.left(pos);
        colorStr = colorStr.right(colorStr.length() - 1);
        if(colorStr.length()>1)
        {
            styleColor.setRgb(colorStr.toInt(0,16));
        }
        //解析文本
        str = str.right(str.length() - pos - 1);
        str = str.left(str.length() - 2);
        dispString = str.right(str.length() - 1);
    }
}

void QTable::resetTable()
{

}
