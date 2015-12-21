#pragma once

#include <qwt_plot.h>
#include "cpustat.h"
#include <qwt_scale_draw.h>
#include <qwt_plot_item.h>
#include <qwt_plot_curve.h>

class TimeScaleDraw : public QwtScaleDraw
{
public:
    TimeScaleDraw(const QTime &base) :
        baseTime(base)
    {
    }
    virtual QwtText label(double v) const
    {
        QTime upTime = baseTime.addSecs(static_cast<int>(v));
        return upTime.toString();
    }
private:
    QTime baseTime;
};

//背景定义
class Background : public QwtPlotItem
{
public:
    Background()
    {
        setZ(0.0);
    }

    virtual int rtti() const
    {
        return QwtPlotItem::Rtti_PlotUserItem;
    }

    virtual void draw(QPainter *painter,
        const QwtScaleMap &, const QwtScaleMap &yMap,
        const QRectF &canvasRect) const
    {
        QColor c(Qt::white);
        QRectF r = canvasRect;

        for (int i = 100; i > 0; i -= 10)
        {
            r.setBottom(yMap.transform(i - 10));
            r.setTop(yMap.transform(i));
            painter->fillRect(r, c);

            c = c.dark(110);
        }
    }
};

//性能曲线定义
class CounterCurve : public QwtPlotCurve
{
public:
    CounterCurve(const QString &title) :
        QwtPlotCurve(title)
    {
        setRenderHint(QwtPlotItem::RenderAntialiased);
    }

    void setColor(const QColor &color)
    {
        QColor c = color;
        c.setAlpha(150);

        setPen(QPen(Qt::NoPen));
        setBrush(c);
    }
};

struct PerfCurves
{
    //名称
    std::string name;
    //CPU曲线
    QwtPlotCurve *cpu_curve = NULL;
    //内存曲线
    QwtPlotCurve *memory_curve = NULL;
};


// #define HISTORY 60 // seconds
// 
// class QwtPlotCurve;
// 
// class CpuPlot : public QwtPlot
// {
//     Q_OBJECT
// public:
//     enum CpuData
//     {
//         User,
//         System,
//         Total,
//         Idle,
// 
//         NCpuData
//     };
// 
//     CpuPlot( QWidget * = 0 );
//     const QwtPlotCurve *cpuCurve( int id ) const
//     {
//         return data[id].curve;
//     }
// 
// protected:
//     void timerEvent( QTimerEvent *e );
// 
// private Q_SLOTS:
//     void legendChecked( const QVariant &, bool on );
// 
// private:
//     void showCurve( QwtPlotItem *, bool on );
// 
//     struct
//     {
//         QwtPlotCurve *curve;
//         double data[HISTORY];
//     } data[NCpuData];
// 
//     double timeData[HISTORY];
// 
//     int dataCount;
//     CpuStat cpuStat;
// };
