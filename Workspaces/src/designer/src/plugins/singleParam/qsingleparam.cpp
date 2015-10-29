#include "qsingleparam.h"
#include <QRect>
QSingleParam::QSingleParam(QWidget *parent) :
    QWidget(parent)
{
    //文本属性
    showType = tr("");
    m_textString = tr("");
    dispString = tr("");
    resultString = tr("");
    textColor.setBlue(255);
    alignment = Qt::AlignCenter;
    //插件框架属性
    QColor Color;
    Color.setAlpha(255);
    Color.setRgb(0);
    backgroundBrush.setColor(Color);
    borderColor.setRgb(0,0,0);
    borderWidth = 1;
    borderStyle = Qt::SolidLine;
    pluginRect.setRect(0,0,0,0);
    m_timer_id = startTimer(500);
    dc = NetComponents::getDataCenter();
}
QSingleParam::~QSingleParam()
{
    killTimer(m_timer_id);
    if(dc)
    {
        delete dc;
        dc = NULL;
    }
}

void QSingleParam::paintEvent(QPaintEvent *)
{
    showPlugin();
}
void QSingleParam::setTextString(const QString string)
{
    m_textString = string;
    update();
}
void QSingleParam::showPluginFrame()
{
    QPainter painter(this);
    QPen pen = setCustomPen(borderStyle,borderColor,borderWidth);
    painter.setPen(pen);
    painter.fillRect(pluginRect,backgroundBrush);
    painter.drawRect(pluginRect);
}
//解析显示字符串，字符串中包括字体、颜色、文本
void QSingleParam::analyzeDisplayStr(QFont &styleFont, QColor &styleColor)
{
    QString str = resultString;
    styleFont = font;
    styleColor = textColor;
    dispString = resultString;
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
void QSingleParam::showPluginText()
{
    QPainter painter(this);
    QFont styleFont;
    QColor styleColor;
    analyzeDisplayStr(styleFont,styleColor);
    painter.setFont(styleFont);
    painter.setPen(styleColor);
    painter.drawText(pluginRect,alignment,dispString);
}
void QSingleParam::timerEvent(QTimerEvent *)
{
    dc->getString(m_textString,showType,resultString);
    update();
}
void QSingleParam::showPlugin()
{
    setPluginRect();
    showPluginFrame();
    showPluginText();
}

void QSingleParam::setTextColor(const QColor Color)
{
    textColor = Color;
    update();
    updateGeometry();
}

void QSingleParam::setBorderColor(const QColor Color)
{
    borderColor = Color;
    update();
    updateGeometry();
}

void QSingleParam::setBorderStyle(const Qt::PenStyle style)
{
    borderStyle = style;
    update();
    updateGeometry();
}

void QSingleParam::setBackgroundBrush(const QBrush brush)
{
    backgroundBrush = brush;
    update();
    updateGeometry();
}

void QSingleParam::setAlignment(const Qt::Alignment flag)
{
    alignment = flag;
    update();
    updateGeometry();
}

void QSingleParam::setFont(const QFont f)
{
    font = f;
    update();
    updateGeometry();
}

void QSingleParam::setBorderWidth(const qint32 width)
{
    borderWidth = width;
    update();
    updateGeometry();
}

void QSingleParam::setShowType(const QString string)
{
    showType = string;
    update();
    updateGeometry();
}

//设置自定义画笔
QPen QSingleParam::setCustomPen(Qt::PenStyle style,QColor color,uint width)
{
    QPen pen;
    pen.setColor(color);
    pen.setStyle(style);
    pen.setWidth((qint32)width);
    return pen;
}

//设置插件矩形区域和背景区域
void QSingleParam::setPluginRect()
{
    QRect rect = this->geometry();
    qint16 w = rect.width();
    qint16 h = rect.height();
    pluginRect.setRect(borderWidth/2,borderWidth/2,w-borderWidth,h-borderWidth);
}

void QSingleParam::resetData()
{
    return;
}
