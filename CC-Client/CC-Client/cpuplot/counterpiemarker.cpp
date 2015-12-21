#include <qpainter.h>
#include <qwt_scale_map.h>
#include <qwt_plot_curve.h>
#include "plotpart.h"
#include "counterpiemarker.h"

CounterPieMarker::CounterPieMarker(PerfCurves* perfCurves, int size, bool isCPUPlot)
    :perfCurves(perfCurves), size(size), isCPUPlot(isCPUPlot)
{
    setZ(1000);
    setRenderHint(QwtPlotItem::RenderAntialiased, true);
}

int CounterPieMarker::rtti() const
{
    return QwtPlotItem::Rtti_PlotUserItem;
}

void CounterPieMarker::draw( QPainter *painter,
    const QwtScaleMap &, const QwtScaleMap &,
    const QRectF &rect ) const
{
    const QwtPlot *counterPlot = static_cast<QwtPlot *> ( plot() );

    const QwtScaleMap yMap = counterPlot->canvasMap( QwtPlot::yLeft );

    const int margin = 5;

    QRectF pieRect;
    pieRect.setX( rect.x() + margin );
    pieRect.setY( rect.y() + margin );
    pieRect.setHeight( yMap.transform( 80.0 ) );
    pieRect.setWidth( pieRect.height() );

    int angle = static_cast<int>( 5760 * 0.75 );
    for (int i = 0; i < size; i++)
    {
        const QwtPlotCurve *curve = isCPUPlot ? perfCurves[i].cpu_curve : perfCurves[i].memory_curve;
        int index = 0;
        if (curve != NULL && curve->dataSize() > 0)
        {
            const int value = static_cast<int>(5760 * curve->sample(0).y() / 100.0);

            painter->save();
            painter->setBrush(QBrush(curve->brush().color(), Qt::SolidPattern));
            if (value != 0)
                painter->drawPie(pieRect, -angle, -value);
            painter->restore();

            if (curve->dataSize() > 2 && index > 0)
            {
                // index 0: total 是其他的总和
                angle += value;
            }
            else
            {
                angle += value;
            }

            index++;
        }
    }    
}
