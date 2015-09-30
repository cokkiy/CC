#include "qsinglestate.h"
#include<qpainter.h>
#include<QPen>
#include<QStylePainter>
#include <QColor>
#include <QRect>
#include <QDebug>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>

QSingleState::QSingleState(QWidget *parent) :
    QWidget(parent)
{
     //插件框架属性
    edgeColor.setRgb(0,0,0);      //设置初始化边框颜色为黑色
    edgeLinewidth = 1;     //设置初始化边框宽度
    edgePenStyle = Qt::NoPen;     //设置初始化边框画笔为不画线
    pluginRect.setRect(0,0,0,0);
    m_bgcolor.setColor(Qt::red);   //初始背景为红色
    m_width=145;
    m_height=95;
    m_timer_id = startTimer(500);
    m_borderBrush.setStyle(Qt::NoBrush);    //设置背景矩形边框无画刷
    m_borderBrush.setColor(Qt::transparent);     //设置背景矩形边框颜色为透明
    connect(this,SIGNAL(backgroundcolorChanged(QBrush)),this,SLOT(update()));
    connect(this,SIGNAL(rectWidthChanged(quint32)),this,SLOT(update()));
    connect(this,SIGNAL(rectHeightChanged(quint32)),this,SLOT(update()));
}

void QSingleState::SetStates(const QString states)
{
    m_states = states;
    update();
}

void QSingleState::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    QPen pen(borderBrush(),1,Qt::NoPen);
    painter.setPen(pen);
    painter.setBrush(backgroundcolor());//设置背景矩形内部颜色

    QRect Rect = this->geometry();
    qint16 w = Rect.width();
    qint16 h = Rect.height();
    pluginRect.setRect(0,0,w-1,h-1);
    painter.drawRect(pluginRect);

    //解析
    int netValue=2;       //传输的参数
    QString picPath;    //图片路径
   QJsonParseError jerr;
   QJsonDocument parse_document = QJsonDocument::fromJson(m_states.toLatin1(), &jerr);
   if(jerr.error == QJsonParseError::NoError)
   {
       if(parse_document.isArray())
       {
           QJsonArray array=parse_document.array();   //将QJsonDocument字符串转换为QJsonArray字符串
           int size=array.size();     //行数
           for(int i=0;i<size;i++)
           {
               QJsonValue value=array.at(i);   //每一行的QJsonObject类型Json字符串
               if(value.isObject())
               {
                   QJsonObject name=value.toObject();
                   double minLimit=name.value("0").toDouble();    //下限取值
                   double maxLimit=name.value("1").toDouble();   //上限取值
                   QString picPath=name["2"].toString();              //图片路径
                   if(netValue>=minLimit && netValue <=maxLimit)
                   {
                       m_pixmap.load(picPath);
                   }
               }
           }
       }
   }
    painter.drawPixmap(0, 0,width(),height(), m_pixmap);

    painter.save();
    SetPluginRect();
    ShowPlugin();
}


void QSingleState::resizeEvent(QResizeEvent *event)
{
    m_width=width()-1;
    m_height=height()-1;
}

void QSingleState::timerEvent(QTimerEvent *event)
{

}

void QSingleState::SetEdgeColor(const QColor Color)
{
    edgeColor = Color;
    update();
    updateGeometry();
}

void QSingleState::SetEdgePenStyle(const Qt::PenStyle style)
{
    edgePenStyle = style;
    update();
    updateGeometry();
}
void QSingleState::SetEdgeLinewidth(const uint width)
{
    edgeLinewidth = width;
    update();
    updateGeometry();
}

void QSingleState::ShowPluginFrame()
{
    QPainter painter(this);
    QPen pen = SetCustomPen(edgePenStyle,edgeColor,edgeLinewidth);
    painter.setPen(pen);
    painter.drawRect(pluginRect);
}
void QSingleState::ShowPlugin()
{
    ShowPluginFrame();
}
//设置自定义画笔
QPen QSingleState::SetCustomPen(Qt::PenStyle style,QColor color,uint width)
{
    QPen pen;
    pen.setColor(color);
    pen.setStyle(style);
    pen.setWidth((int)width);
    return pen;
}
//设置插件矩形区域和背景区域
void QSingleState::SetPluginRect()
{
    QRect Rect = this->geometry();
    qint16 w = Rect.width();
    qint16 h = Rect.height();
    pluginRect.setRect(0,0,w-1,h-1);
}


