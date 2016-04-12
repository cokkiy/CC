#ifndef DETAILDIALOG_H
#define DETAILDIALOG_H

#include <QDialog>
#include "ui_detaildialog.h"
#include "cpuplot/plotpart.h"
class StationInfo;
class QwtPlot;
class CounterPieMarker;
class QTimerEvent;
class StationManager;
//性能计数器最多个数
#define  MaxCountOfCounter 20

class DetailDialog : public QDialog
{
	Q_OBJECT

public:
	DetailDialog(StationInfo* station, QWidget *parent = 0);

	void setButtonState();

	void setupPlot();

	~DetailDialog();

	protected slots:
	//开机
	void on_powerOnPushButton_clicked();
	//关机
	void on_powerOffPushButton_clicked();
	//重启
	void on_rebootPushButton_clicked();
	//屏幕快照
	void on_screenCapturePushButton_clicked();
	//工作站状态变化
	void stationStateChanged(const QObject* obj);

private:
	Ui::DetailDialog ui;
	StationInfo* station;
	void setPlotStyle(QwtPlot* plot, QString title);
	void timerEvent(QTimerEvent * e);

	//重置进程表格内容
	void resetTableWidget(int columnCount);
	//更新进程信息
	void UpdateMonitoredProcessInfo();

	//性能曲线数组
	PerfCurves perfCurves[MaxCountOfCounter];
	CounterPieMarker *cpuPie, *memoryPie;
	QwtPlot* cpuPlot;
	QwtPlot* memoryPlot;
	double timeData[100];
	//工作站管理类
	StationManager* manager;
};

#endif // DETAILDIALOG_H
