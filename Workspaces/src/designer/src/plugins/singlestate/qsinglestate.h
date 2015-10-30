#ifndef QSINGLESTATE_H
#define QSINGLESTATE_H

#include <QWidget>
#include <QtGui>
#include <Net/NetComponents>

class QSingleState : public QWidget
{
    Q_OBJECT

    //数据属性
    Q_PROPERTY(QString data READ getData WRITE setData)
    //状态设置
    Q_PROPERTY(QString stateConfig READ GetStates WRITE SetStates)
    //背景属性
    Q_PROPERTY(QBrush backgroundColor READ backgroundColor WRITE
               setbackgroundcolor NOTIFY backgroundcolorChanged)
    //边框属性
    Q_PROPERTY(QColor borderColor READ GetEdgeColor WRITE SetEdgeColor)
    Q_PROPERTY(uint borderWidth READ GetEdgeLinewidth WRITE SetEdgeLinewidth)
    Q_PROPERTY(Qt::PenStyle borderStyle READ GetEdgePenStyle WRITE SetEdgePenStyle)
    //2、设置控件边框，控件定位的必要变量坐标
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)
    //设置图元大小策略的属性
    Q_PROPERTY(QSizePolicy sizePolicy READ sizePolicy WRITE setSizePolicy)
    Q_PROPERTY(QSize minimumSize READ minimumSize WRITE setMinimumSize)
    Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize)
    Q_PROPERTY(QSize sizeIncrement READ sizeIncrement WRITE setSizeIncrement)
    Q_PROPERTY(QSize baseSize READ baseSize WRITE setBaseSize)

     void paintEvent(QPaintEvent *event);//重写绘图函数
     void resizeEvent(QResizeEvent * event);//重画事件

public:
     QSingleState(QWidget *parent = 0);
     ~QSingleState();

     QString GetStates() const
     {
         return m_states;
     }
     void SetStates(const QString states);

    //设置判断参数
     void setData(const QString &newData)
     {
         m_data=newData;
         m_bvalidData = false;
     }
     QString getData() const
     {
         return m_data;
     }

    //设置插件在画布x,y坐标
     void setX(quint32 x)
     {
         this->move(x,y());
     }
     void setY(qint32 y)
     {
         this->move(x(),y);
     }

     //框架属性
     QColor GetEdgeColor() const
     {
         return borderColor;
     }
     void SetEdgeColor(const QColor Color);

    Qt::PenStyle GetEdgePenStyle() const
    {
        return borderStyle;
    }
    void SetEdgePenStyle(const Qt::PenStyle style);

    qint16 GetEdgeLinewidth() const
    {
        return borderWidth;
    }
    void SetEdgeLinewidth(const uint width);

    //设置背景颜色
    void setbackgroundcolor(const QBrush & bgcolor)
    {
        m_bgcolor=bgcolor;
        emit backgroundcolorChanged(bgcolor);
    }
    QBrush backgroundColor() const
    {
        return m_bgcolor;
    }

     //重置画的背景矩形边框的属性
    void setBorderBrush(const QBrush & brush)
    {
        m_borderBrush=brush;
    }
    QBrush borderBrush() const
    {
        return m_borderBrush;
    }
    //设置边框宽度
    void setRectWidth(qint32 width)
    {
        m_width=width;
        emit rectWidthChanged(width);
    }
    qint32 rectWidth()
    {        return m_width;        }

   //设置边框高度
    void setRectHeight(qint32 height)
    {
        m_height=height;
        emit rectHeightChanged(height);
    }
    qint32 rectHeight()
    {
        return m_height;
    }

private:
    //插件框架属性
    QString m_data;   //参数
    double netFloatValue;   //判断参数
    bool m_bvalidData;   //是否获取有效参数数据

    QString m_states;  //传输JsonArray字符串
    DataCenterInterface* m_dc;

    int m_timer_id;
    qint32 m_width,m_height;
    QPen edgePen;     //边框画笔
    QColor borderColor;     //插件边框颜色
    Qt::PenStyle borderStyle;     //插件边框画笔样式
    uint borderWidth;      //插件边框宽度


    QPixmap m_pixmap;
    QBrush m_bgcolor;   //背景色
    QBrush m_borderBrush;    //背景矩形的边框画刷颜色和宽度

    QRect pluginRect;    //插件大小区域
    void ShowPluginFrame();    //显示插件矩形区域
    void ShowPlugin();   //显示插件
    QPen SetCustomPen(Qt::PenStyle style,QColor color,uint width);   //设置自定义画笔
    void SetPluginRect();  //设置插件矩形区域和背景区域

signals:
    void backgroundcolorChanged(const QBrush &);
    void rectHeightChanged(quint32);
    void rectWidthChanged(quint32);

protected slots:
    void timerEvent(QTimerEvent *event);
};

#endif
