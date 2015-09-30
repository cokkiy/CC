#include "rollinggraph.h"
//#include <Net/NetComponents>
#include "qcustomplot.h"
class rollinggraphPrivate :QObject{
public:
    explicit rollinggraphPrivate(QWidget*);
    virtual ~rollinggraphPrivate();
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
//    Net m_dci;
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
//rollinggraphPrivate::rollinggraphPrivate(QWidget*wgt):m_paramX("cosx"),m_xUp(50),m_xDown(0),
//    m_paramY("cosy"),m_yUp(15),m_yDown(-15),m_curveColor(Qt::blue),
//    m_curveWidth(3),m_dci(this)
rollinggraphPrivate::rollinggraphPrivate(QWidget*wgt):m_paramX("cosx"),m_xUp(50),m_xDown(0),
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
}
rollinggraphPrivate::~rollinggraphPrivate()
{
    if(m_plot)
    {
        delete m_plot;
        m_plot = NULL;
    }
}

void rollinggraphPrivate::setXUp(const double xu)
{
    m_xUp = xu;
    m_plot->xAxis->setRangeUpper(xUp());
    update();
}
void rollinggraphPrivate::setXDown(const double xd)
{
    m_xDown = xd;
    m_plot->xAxis->setRangeLower(xDown());
    update();
}
void rollinggraphPrivate::setYUp(const double yu)
{
    m_yUp = yu;
    m_plot->yAxis->setRangeUpper(yUp());
    update();
}
void rollinggraphPrivate::setYDown(const double yd)
{
    m_yDown = yd;
    m_plot->yAxis->setRangeLower(yDown());
    update();
}
void rollinggraphPrivate::setCurveColor(const QColor cc)
{
    m_curveColor=cc;
    m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, curveColor(),curveWidth()));
    update();
}

void rollinggraphPrivate::setCurveWidth(const int cw)
{
    m_curveWidth=cw;
    m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, curveColor(),curveWidth()));
    update();
}

void rollinggraphPrivate::setPlot()
{
    m_plot->setGeometry(parent->rect());
    m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, curveColor(),curveWidth()));
    m_plot->xAxis->setLabel("x");
    m_plot->yAxis->setLabel("y");
    m_plot->xAxis->setRange(xDown(), xUp());
    m_plot->yAxis->setRange(yDown(), yUp());
}

void rollinggraphPrivate::update()
{
    m_plot->replot();
}
void rollinggraphPrivate::getData()
{
//    QVector<double> x, y;
//    m_dci.getHistoryData(paramX(),x);
//    m_dci.getHistoryData(paramY(),y);
    QVector<double>x(200),y(200);
    for(int i=0;i<200;++i)
    {

        x[i] = i;
        y[i] =sin((((double)x[i])/50.0)*2*3.1415);
    }

    foreach (double tx, x) {
        m_x.append(tx);
        qDebug()<<QString("insert x %1 \n").arg(tx);
    }
    foreach (double ty, y) {
        m_y.append(ty);
        qDebug()<<QString("insert y %1 \n").arg(ty);
    }
    if(m_plot&&m_plot->graph(0)){
        m_plot->graph(0)->setData(m_x, m_y);
        m_plot->replot();
    }
}

rollinggraph::rollinggraph(QWidget *parent) :
    QWidget(parent), d_ptr(new rollinggraphPrivate(this))
{
    m_timer_id = startTimer(500);
    qDebug()<<QString("rollinggraph startTimer %1 \n").arg(m_timer_id);
}
rollinggraph::~rollinggraph()
{
    killTimer(m_timer_id);
    qDebug()<<QString("rollinggraph killTimer %1 \n").arg(m_timer_id);
}

void rollinggraph::timerEvent(QTimerEvent *event)
{
    Q_D(rollinggraph);
    d->getData();
    d->update();
}

void rollinggraph::paintEvent(QPaintEvent *event)
{
    Q_D(rollinggraph);
    d->setPlot();
    d->update();
    qDebug()<<"QCustomPlot::paintEvent(event)";
}

QString rollinggraph::paramX()const
{
    Q_D(const rollinggraph);
    return d->paramX();
}

void rollinggraph::setParamX(const QString px)
{
    Q_D(rollinggraph);
    d->setParamX(px);
}

double rollinggraph::xUp()const
{
    Q_D(const rollinggraph);
    return d->xUp();
}

void rollinggraph::setXUp(const double xu)
{
    Q_D(rollinggraph);
    d->setXUp(xu);
}

double rollinggraph::xDown()const
{
    Q_D(const rollinggraph);
    return d->xDown();
}

void rollinggraph::setXDown(const double xd)
{
    Q_D(rollinggraph);
    d->setXDown(xd);
}

QString rollinggraph::paramY()const
{
    Q_D(const rollinggraph);
    return d->paramY();
}

void rollinggraph::setParamY(const QString py)
{
    Q_D(rollinggraph);
    d->setParamY(py);
}
double rollinggraph::yUp()const
{
    Q_D(const rollinggraph);
    return d->yUp();
}

void rollinggraph::setYUp(const double yu)
{
    Q_D(rollinggraph);
    d->setYUp(yu);
}

double rollinggraph::yDown()const
{
    Q_D(const rollinggraph);
    return d->yDown();
}

void rollinggraph::setYDown(const double yd)
{
    Q_D(rollinggraph);
    d->setYDown(yd);
}

QColor rollinggraph::curveColor()const
{
    Q_D(const rollinggraph);
    return d->curveColor();
}

void rollinggraph::setCurveColor(const QColor cc)
{
    Q_D(rollinggraph);
    d->setCurveColor(cc);
}

int rollinggraph::curveWidth()const
{
    Q_D(const rollinggraph);
    return d->curveWidth();
}

void rollinggraph::setCurveWidth(const int cw)
{
    Q_D(rollinggraph);
    d->setCurveWidth(cw);
}
