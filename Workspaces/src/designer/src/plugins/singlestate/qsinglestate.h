#ifndef QSINGLESTATE_H
#define QSINGLESTATE_H

#include <QWidget>
#include <QtGui>

class QSingleState : public QWidget
{
    Q_OBJECT
//数据属性
    Q_PROPERTY(QString states READ GetStates WRITE SetStates /*NOTIFY rectHeightChanged*/)
    Q_PROPERTY(QString data READ textString WRITE setTextString)

//背景属性
    Q_PROPERTY(QBrush backgroundcolor READ backgroundcolor WRITE
               setbackgroundcolor NOTIFY backgroundcolorChanged)

//边框属性
    Q_PROPERTY(QColor edgeColor READ GetEdgeColor WRITE SetEdgeColor) 
    Q_PROPERTY(uint edgeLinewidth READ GetEdgeLinewidth WRITE SetEdgeLinewidth)
    Q_PROPERTY(qint32 rectWidth READ rectWidth WRITE setRectWidth NOTIFY rectWidthChanged)
    Q_PROPERTY(qint32 rectHeight READ rectHeight WRITE setRectHeight NOTIFY rectHeightChanged)
    Q_PROPERTY(Qt::PenStyle edgePenStyle READ GetEdgePenStyle WRITE SetEdgePenStyle)

//位置属性
    Q_PROPERTY(qint32 x READ x WRITE setX )
    Q_PROPERTY(qint32 y READ y WRITE setY )

     void paintEvent(QPaintEvent *event);//重写绘图函数
     void resizeEvent(QResizeEvent * event);//重画事件

public:
     QString m_states;
     QSingleState(QWidget *parent = 0);

//设置插件在画布x,y坐标
     void setX(quint32 x)
     {
         this->move(x,y());
     }
     void setY(qint32 y)
     {
         this->move(x(),y);
     }

     QString GetStates() const {return m_states;}
     void SetStates(const QString states);

     QString textString() const {return m_textString;}
     void setTextString(const QString string);

     //框架属性
     QColor GetEdgeColor() const {return edgeColor;}
     void SetEdgeColor(const QColor Color);

    Qt::PenStyle GetEdgePenStyle() const {return edgePenStyle;}
    void SetEdgePenStyle(const Qt::PenStyle style);

    qint16 GetEdgeLinewidth() const {return edgeLinewidth;}
    void SetEdgeLinewidth(const uint width);

    //设置背景颜色
    void setbackgroundcolor(const QBrush & bgcolor)
    {
        m_bgcolor=bgcolor;
        emit backgroundcolorChanged(bgcolor);
    }
    QBrush backgroundcolor() const
    {        return m_bgcolor;    }
    void setBorderBrush(const QBrush & brush) //重置画的背景矩形边框的属性
    {  m_borderBrush=brush;    }
    QBrush borderBrush() const
    {        return m_borderBrush;    }
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
    {        return m_height;     }

private:
    //插件框架属性
    qint32 m_width,m_height;
    QPen edgePen;     //边框画笔
    QColor edgeColor;     //插件边框颜色
    Qt::PenStyle edgePenStyle;       //插件边框画笔样式
    uint edgeLinewidth;      //插件边框宽度
    int m_timer_id;


    QPixmap m_pixmap;
    QString m_textString;//输入参数

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
