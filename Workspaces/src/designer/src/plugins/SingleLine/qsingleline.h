#ifndef QSINGLELINE_H
#define QSINGLELINE_H

#include <QWidget>
#include <QtGui>

class QSingleLine : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor lineColor READ GetLineColor WRITE SetLineColor)
    Q_PROPERTY(Qt::PenStyle linePenStyle READ GetLinePenStyle WRITE SetLinePenStyle)
    Q_PROPERTY(uint lineWidth READ GetLineWidth WRITE SetLineWidth)
    Q_PROPERTY(uint lineLength READ GetLineLength WRITE SetLineLength)
    Q_PROPERTY(qint32 rotate READ rotate WRITE setRotate NOTIFY rotateChanged RESET unsetRotate)
    Q_PROPERTY(qint32 x READ x WRITE setX )
    Q_PROPERTY(qint32 y READ y WRITE setY )
protected:
     void paintEvent(QPaintEvent *event);
public:
     QColor GetLineColor() const {return lineColor;}
     void SetLineColor(const QColor Color);

     Qt::PenStyle GetLinePenStyle() const {return linePenStyle;}
     void SetLinePenStyle(const Qt::PenStyle style);

     qint16 GetLineWidth() const {return lineWidth;}
     void SetLineWidth(const uint width);

     qint16 GetLineLength() const {return lineLength;}
     void SetLineLength(const uint length);


     void setX(quint32 x)//设置插件在画布x,y坐标
     {
         this->move(x,y());
     }
     void setY(qint32 y)
     {
         this->move(x(),y);
     }

     void setRotate(quint32 rotate);  //旋转角度
     qint32 rotate() const
     {
         return m_rotate;
     }
     void unsetRotate();
signals:
    void rotateChanged(qint32);

private:
     //直线属性
    QColor lineColor;//直线颜色
    Qt::PenStyle linePenStyle;//直线画笔样式
    uint lineWidth;//直线宽度
    uint lineLength ;//直线长度
    qint32 m_rotate;//直线旋转角度
    QPen linePen;//直线画笔
    QPen SetCustomPen(Qt::PenStyle style,QColor color,uint width);//设置自定义画笔

public:
    QSingleLine(QWidget *parent = 0);
    ~QSingleLine();
};

#endif
