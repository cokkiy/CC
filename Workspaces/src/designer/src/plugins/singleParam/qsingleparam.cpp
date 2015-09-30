#include "qsingleparam.h"
#include <QRect>
QSingleParam::QSingleParam(QWidget *parent) :
    QWidget(parent)
{
    //文本属性
    showType = tr("");
    m_textString = tr("");
    dispString = m_textString;
    textColor.setBlue(255);
    alignment = Qt::AlignCenter;
    //插件框架属性
    backgroundColor.setRgb(0,0,0,0);
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
    SetPluginRect();
    ShowPlugin();
}
void QSingleParam::setTextString(const QString string)
{
    m_textString = string;
    update();
}
void QSingleParam::ShowPluginFrame()
{
    QPainter painter(this);
    QPen pen = SetCustomPen(borderStyle,borderColor,borderWidth);
    painter.setPen(pen);
    painter.fillRect(pluginRect,backgroundColor);
    painter.drawRect(pluginRect);
}
void QSingleParam::ShowPluginText()
{
    QPainter painter(this);
    painter.setFont(font);
    painter.setPen(textColor);
    painter.drawText(pluginRect,alignment,dispString);
}
void QSingleParam::timerEvent(QTimerEvent *event)
{
    dc->getString(m_textString,showType,dispString);
}
void QSingleParam::ShowPlugin()
{
    ShowPluginFrame();
    ShowPluginText();
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

void QSingleParam::setBackgroundColor(const QColor Color)
{
    backgroundColor = Color;
    update();
    updateGeometry();
}



void QSingleParam::setAlignment(const Qt::Alignment flag)
{
    alignment = flag;
    update();
    updateGeometry();
}

void QSingleParam::setFont(const QFont Font)
{
    font = Font;
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
QPen QSingleParam::SetCustomPen(Qt::PenStyle style,QColor color,uint width)
{
    QPen pen;
    pen.setColor(color);
    pen.setStyle(style);
    pen.setWidth((qint32)width);
    return pen;
}

//设置插件矩形区域和背景区域
void QSingleParam::SetPluginRect()
{
    QRect Rect = this->geometry();
    qint16 w = Rect.width();
    qint16 h = Rect.height();
    pluginRect.setRect(1,1,w-3,h-3);
}

