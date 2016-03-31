#ifndef DETAILDIALOG_H
#define DETAILDIALOG_H

#include <QDialog>
#include "ui_detaildialog.h"
#include "cpuplot/plotpart.h"
class StationInfo;
class QwtPlot;
class CounterPieMarker;
class QTimerEvent;
//性能计数器最多个数
#define  MaxCountOfCounter 20

class DetailDialog : public QDialog
{
	Q_OBJECT

public:
	DetailDialog(StationInfo* station, QWidget *parent = 0);

	void setupPlot();

	~DetailDialog();

private:
	Ui::DetailDialog ui;
	StationInfo* station;
	void setPlotStyle(QwtPlot* plot, QString title);
	void timerEvent(QTimerEvent * e);
	//性能曲线数组
	PerfCurves perfCurves[MaxCountOfCounter];
	CounterPieMarker *cpuPie, *memoryPie;
	QwtPlot* cpuPlot;
	QwtPlot* memoryPlot;
	double timeData[100];
};

#endif // DETAILDIALOG_H
