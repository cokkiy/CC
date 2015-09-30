#include "table.h"
#include <QRect>

QTable::QTable(QWidget *parent) :
    QWidget(parent)
{
    //文本属性
    m_textString = tr("");
    dispString = m_textString;
    //插件框架属性
    backgroundColor.setRgb(0,0,0,0);
    borderColor.setRgb(0,0,0);
    borderWidth = 1;
    borderStyle = Qt::SolidLine;
    pluginRect.setRect(0,0,0,0);
    m_timer_id = startTimer(500);
    dc = NetComponents::getDataCenter();
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

void QTable::paintEvent(QPaintEvent *)
{
    SetPluginRect();
    ShowPlugin();
}
void QTable::setTextString(const QString string)
{
    m_textString = string;
    update();
}
void QTable::ShowPluginFrame()
{
    QPainter painter(this);
    QPen pen = SetCustomPen(borderStyle,borderColor,borderWidth);
    painter.setPen(pen);
    painter.fillRect(pluginRect,backgroundColor);
    painter.drawRect(pluginRect);
}
void QTable::ShowPluginText()
{
    QPainter painter(this);
    //painter.setFont(font);
    //painter.setPen(textColor);
    //painter.drawText(pluginRect,alignment,dispString);
}
void QTable::timerEvent(QTimerEvent *event)
{
    //dc->getString(m_textString,showType,dispString);
}
void QTable::ShowPlugin()
{
    ShowPluginFrame();
    ShowPluginText();
}


void QTable::setBorderColor(const QColor Color)
{
    borderColor = Color;
    update();
    updateGeometry();
}

void QTable::setBorderStyle(const Qt::PenStyle style)
{
    borderStyle = style;
    update();
    updateGeometry();
}

void QTable::setBackgroundColor(const QColor Color)
{
    backgroundColor = Color;
    update();
    updateGeometry();
}

void QTable::setBorderWidth(const qint32 width)
{
    borderWidth = width;
    update();
    updateGeometry();
}



//设置自定义画笔
QPen QTable::SetCustomPen(Qt::PenStyle style,QColor color,uint width)
{
    QPen pen;
    pen.setColor(color);
    pen.setStyle(style);
    pen.setWidth((qint32)width);
    return pen;
}

//设置插件矩形区域和背景区域
void QTable::SetPluginRect()
{
    QRect Rect = this->geometry();
    qint16 w = Rect.width();
    qint16 h = Rect.height();
    pluginRect.setRect(1,1,w-3,h-3);
}
