#include "simplecurve.h"
#include <Net/NetComponents>
#include "qcustomplot.h"
class SimpleCurvePrivate :QObject{
public:
    explicit SimpleCurvePrivate(QWidget*);
    virtual ~SimpleCurvePrivate();
public:
    void setPlot();
    void update();
    void getData();
public:
    QString paramX()const{return m_paramX;}
    void setParamX(const QString px){m_paramX=px;}
    double xUp()const{return m_xUp;}
    void setXUp(const double xu);
    double xDown()const{return m_xDown;}
    void setXDown(const double xd);
    QString paramY()const{return m_paramY;}
    void setParamY(const QString py){m_paramY=py;}
    double yUp()const{return m_yUp;}
    void setYUp(const double yu);
    double yDown()const{return m_yDown;}
    void setYDown(const double yd);
    QColor curveColor()const{return m_curveColor;}
    void setCurveColor(const QColor cc);
    int curveWidth()const{return m_curveWidth;}
    void setCurveWidth(const int cw);
private:
    bool m_bAlready;
    int m_pos;
private:
    QCustomPlot* m_plot;
    QWidget * parent;
    //int m_timer_id;
    DataCenterInterface* m_dci_x;
    DataCenterInterface* m_dci_y;
    QVector<double> m_x, m_y;
public:
    QString m_paramX;
    double m_xUp;
    double m_xDown;
    QString m_paramY;
    double m_yUp;
    double m_yDown;
    QColor m_curveColor;
    int m_curveWidth;
};
SimpleCurvePrivate::SimpleCurvePrivate(QWidget*wgt):m_paramX("cosx"),m_xUp(50),m_xDown(0),
    m_paramY("cosy"),m_yUp(15),m_yDown(-15),m_curveColor(Qt::blue),
    m_curveWidth(3)
{
    parent = wgt;
    m_plot = new QCustomPlot(parent);
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    m_plot->setVisible(true);
    m_plot->addGraph();
    m_plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, curveColor(),curveWidth()));
    m_plot->xAxis->setLabel("x");
    m_plot->yAxis->setLabel("y");
    m_plot->setGeometry(parent->rect());
    m_dci_x = NetComponents::getDataCenter();
    m_dci_y = NetComponents::getDataCenter();
}
SimpleCurvePrivate::~SimpleCurvePrivate()
{
    if(m_plot)
    {
        delete m_plot;
        m_plot = NULL;
    }
    delete m_dci_x;
    delete m_dci_y;
}

void SimpleCurvePrivate::setXUp(const double xu)
{
    m_xUp = xu;
    m_plot->xAxis->setRangeUpper(xUp());
    update();
}
void SimpleCurvePrivate::setXDown(const double xd)
{
    m_xDown = xd;
    m_plot->xAxis->setRangeLower(xDown());
    update();
}
void SimpleCurvePrivate::setYUp(const double yu)
{
    m_yUp = yu;
    m_plot->yAxis->setRangeUpper(yUp());
    update();
}
void SimpleCurvePrivate::setYDown(const double yd)
{
    m_yDown = yd;
    m_plot->yAxis->setRangeLower(yDown());
    update();
}
void SimpleCurvePrivate::setCurveColor(const QColor cc)
{
    m_curveColor=cc;
    m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, curveColor(),curveWidth()));
    update();
}

void SimpleCurvePrivate::setCurveWidth(const int cw)
{
    m_curveWidth=cw;
    m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, curveColor(),curveWidth()));
    update();
}

void SimpleCurvePrivate::setPlot()
{
    m_plot->setGeometry(parent->rect());
    m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, curveColor(),curveWidth()));
    m_plot->xAxis->setLabel("x");
    m_plot->yAxis->setLabel("y");
    m_plot->xAxis->setRange(xDown(), xUp());
    m_plot->yAxis->setRange(yDown(), yUp());
}

void SimpleCurvePrivate::update()
{
    m_plot->replot();
}
void SimpleCurvePrivate::getData()
{
    QVector<double> x, y;
    m_dci_x->getHistoryData(paramX(),x);
    m_dci_y->getHistoryData(paramY(),y);
    foreach (double tx, x) {
        m_x.append(tx);
        //qDebug()<<QString("insert x %1 \n").arg(tx);
    }
    foreach (double ty, y) {
        m_y.append(ty);
        //qDebug()<<QString("insert y %1 \n").arg(ty);
    }
    if(m_plot&&m_plot->graph(0)){
        m_plot->graph(0)->setData(m_x, m_y);
        m_plot->replot();
    }
}

SimpleCurve::SimpleCurve(QWidget *parent) :
    QWidget(parent), d_ptr(new SimpleCurvePrivate(this))
{
    m_timer_id = startTimer(500);
    qDebug()<<QString("SimpleCurve startTimer %1 \n").arg(m_timer_id);
}
SimpleCurve::~SimpleCurve()
{
    killTimer(m_timer_id);
    qDebug()<<QString("SimpleCurve killTimer %1 \n").arg(m_timer_id);
}

void SimpleCurve::timerEvent(QTimerEvent *event)
{
    Q_D(SimpleCurve);
    d->getData();
    d->update();
}

void SimpleCurve::paintEvent(QPaintEvent *event)
{
    Q_D(SimpleCurve);
    d->setPlot();
    d->update();
    qDebug()<<"QCustomPlot::paintEvent(event)";
}

QString SimpleCurve::paramX()const
{
    Q_D(const SimpleCurve);
    return d->paramX();
}

void SimpleCurve::setParamX(const QString px)
{
    Q_D(SimpleCurve);
    d->setParamX(px);
}

double SimpleCurve::xUp()const
{
    Q_D(const SimpleCurve);
    return d->xUp();
}

void SimpleCurve::setXUp(const double xu)
{
    Q_D(SimpleCurve);
    d->setXUp(xu);
}

double SimpleCurve::xDown()const
{
    Q_D(const SimpleCurve);
    return d->xDown();
}

void SimpleCurve::setXDown(const double xd)
{
    Q_D(SimpleCurve);
    d->setXDown(xd);
}

QString SimpleCurve::paramY()const
{
    Q_D(const SimpleCurve);
    return d->paramY();
}

void SimpleCurve::setParamY(const QString py)
{
    Q_D(SimpleCurve);
    d->setParamY(py);
}
double SimpleCurve::yUp()const
{
    Q_D(const SimpleCurve);
    return d->yUp();
}

void SimpleCurve::setYUp(const double yu)
{
    Q_D(SimpleCurve);
    d->setYUp(yu);
}

double SimpleCurve::yDown()const
{
    Q_D(const SimpleCurve);
    return d->yDown();
}

void SimpleCurve::setYDown(const double yd)
{
    Q_D(SimpleCurve);
    d->setYDown(yd);
}

QColor SimpleCurve::curveColor()const
{
    Q_D(const SimpleCurve);
    return d->curveColor();
}

void SimpleCurve::setCurveColor(const QColor cc)
{
    Q_D(SimpleCurve);
    d->setCurveColor(cc);
}

int SimpleCurve::curveWidth()const
{
    Q_D(const SimpleCurve);
    return d->curveWidth();
}

void SimpleCurve::setCurveWidth(const int cw)
{
    Q_D(SimpleCurve);
    d->setCurveWidth(cw);
}
