#ifndef ROLLINGGRAPH_H
#define ROLLINGGRAPH_H

#include <QWidget>
class rollinggraphPrivate;
class rollinggraph : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)
    Q_PROPERTY(QString paramX READ paramX WRITE setParamX)
    Q_PROPERTY(double xUp READ xUp WRITE setXUp)
    Q_PROPERTY(double xDown READ xDown WRITE setXDown)
    Q_PROPERTY(QString paramY READ paramY WRITE setParamY)
    Q_PROPERTY(double yUp READ yUp WRITE setYUp)
    Q_PROPERTY(double yDown READ yDown WRITE setYDown)
    Q_PROPERTY(QColor curveColor READ curveColor WRITE setCurveColor)
    Q_PROPERTY(int curveWidth READ curveWidth WRITE setCurveWidth)
public:
    rollinggraph(QWidget *parent = 0);
    virtual ~rollinggraph();
public:
    QString paramX()const;
    void setParamX(const QString);
    double xUp()const;
    void setXUp(const double);
    double xDown()const;
    void setXDown(const double);
    QString paramY()const;
    void setParamY(const QString);
    double yUp()const;
    void setYUp(const double);
    double yDown()const;
    void setYDown(const double);
    QColor curveColor()const;
    void setCurveColor(const QColor);
    int curveWidth()const;
    void setCurveWidth(const int);
protected slots:
    void timerEvent(QTimerEvent *event);
protected:
    void paintEvent(QPaintEvent *event);    
private:
    int m_timer_id;
private:
    QScopedPointer<rollinggraphPrivate> d_ptr;
    Q_DECLARE_PRIVATE(rollinggraph)
    Q_DISABLE_COPY(rollinggraph)
};
QT_END_NAMESPACE
#endif
