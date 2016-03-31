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

    int angleOfTotalAndIdle = static_cast<int>( 5760 * 0.75 ); //total和idle角度 total+idle=100%
	int angleOfApp= static_cast<int>(5760 * 0.75); //监视程序角度
	int indexOfIdle = 0; //Idle使用量索引
	for (int i = 0; i < size; i++)
	{
		const QwtPlotCurve *curve = isCPUPlot ? perfCurves[i].cpu_curve : perfCurves[i].memory_curve;
		if (curve == NULL)
		{
			indexOfIdle = i - 1;
			break;
		}
	}
    for (int i = 0; i < size; i++)
    {
        const QwtPlotCurve *curve = isCPUPlot ? perfCurves[i].cpu_curve : perfCurves[i].memory_curve;
        if (curve != NULL && curve->dataSize() > 0)
        {
			const int value = static_cast<int>(5760 * curve->sample(0).y() / 100.0);
			painter->save();
			painter->setBrush(QBrush(curve->brush().color(), Qt::SolidPattern));
			if (i == 0) //total
			{
				if (value != 0)
					painter->drawPie(pieRect, -angleOfTotalAndIdle, -value);
				angleOfTotalAndIdle += value;
			}
			else if(i==indexOfIdle) //idle
			{
				if (value != 0)
					painter->drawPie(pieRect, -angleOfTotalAndIdle, -value);
			}
			else //其他app
			{
				if (value != 0)
					painter->drawPie(pieRect, -angleOfApp, -value);
				angleOfApp += value;
			}
			painter->restore();			
        }
    }    
}
