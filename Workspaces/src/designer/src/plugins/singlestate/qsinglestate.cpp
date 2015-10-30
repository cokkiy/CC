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
#include <Net/NetComponents>

QSingleState::QSingleState(QWidget *parent) :
    QWidget(parent)
{
     //插件框架属性
    borderColor.setRgb(0,0,0);      //设置初始化边框颜色为黑色
    borderWidth = 1;     //设置初始化边框宽度
    borderStyle = Qt::SolidLine;     //设置初始化边框画笔为不画线
    pluginRect.setRect(0,0,0,0);
    m_bgcolor.setColor(Qt::red);   //初始背景为红色
    m_width=150;
    m_height=100;

    m_borderBrush.setStyle(Qt::NoBrush);    //设置背景矩形边框无画刷
    m_borderBrush.setColor(Qt::transparent);     //设置背景矩形边框颜色为透明

    m_timer_id = startTimer(500);
    m_dc = NetComponents::getDataCenter();
    netFloatValue = 0;
    m_bvalidData = false;

    connect(this,SIGNAL(backgroundcolorChanged(QBrush)),this,SLOT(update()));
    connect(this,SIGNAL(rectWidthChanged(quint32)),this,SLOT(update()));
    connect(this,SIGNAL(rectHeightChanged(quint32)),this,SLOT(update()));
}

QSingleState::~QSingleState()
{
    killTimer(m_timer_id);//删除定时器
    if(m_dc)
    {
        delete m_dc;
        m_dc = NULL;
    }
}

void QSingleState::resizeEvent(QResizeEvent *event)
{
    m_width=width()-1;
    m_height=height()-1;
}

//定时器事件
void QSingleState::timerEvent(QTimerEvent *event)
{
    ////使用数据处理中心的接口方法
    double ret;
    if(m_dc->getValue(m_data,ret))
    {
        netFloatValue = ret;
        m_bvalidData = true;
        update();
    }
}

void QSingleState::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    QPen pen(borderBrush(),1,Qt::NoPen);
    painter.setPen(pen);
    painter.setBrush(backgroundColor());//设置背景矩形内部颜色

    QRect Rect = this->geometry();
    qint16 w = Rect.width();
    qint16 h = Rect.height();
    pluginRect.setRect(0,0,w-1,h-1);
    painter.drawRect(pluginRect);

    //解析JsonArray字符串
    QString picPath;      //图片路径
    QJsonParseError jerr;  //解析报错
    QJsonDocument parse_document = QJsonDocument::fromJson(m_states.toUtf8(), &jerr);
    if(jerr.error == QJsonParseError::NoError&&m_bvalidData)
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
                    double minLimit=name.value("down").toDouble();    //下限取值
                    double maxLimit=name.value("up").toDouble();   //上限取值
                    QString picPath=name["picPath"].toString();              //图片路径
                    if(netFloatValue>=minLimit && netFloatValue <=maxLimit)
                    {
                        m_pixmap.load(picPath);
                        painter.drawPixmap(0, 0,width(),height(), m_pixmap); //打开图片并显示在当前对话框
                        painter.save();

                        m_bvalidData = false;
                        break;
                    }
                }
            }
        }
    }

    SetPluginRect();
    ShowPlugin();
}

void QSingleState::SetStates(const QString states)
{
    m_states = states;
    update();
}

void QSingleState::SetEdgeColor(const QColor Color)
{
    borderColor = Color;
    update();
    updateGeometry();
}

void QSingleState::SetEdgePenStyle(const Qt::PenStyle style)
{
    borderStyle = style;
    update();
    updateGeometry();
}

void QSingleState::SetEdgeLinewidth(const uint width)
{
    borderWidth = width;
    update();
    updateGeometry();
}

void QSingleState::ShowPluginFrame()
{
    QPainter painter(this);
    QPen pen = SetCustomPen(borderStyle,borderColor,borderWidth);
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


