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
	//查看全部进程信息
	void on_viewALlPushButton_clicked();

	//屏幕快照
	void on_screenCapturePushButton_clicked();
	//工作站状态变化
	void stationStateChanged(const QObject* obj);

    virtual void closeEvent(QCloseEvent * event);

private:
	Ui::DetailDialog ui;
	StationInfo* station;
    bool isClosed;
	void setPlotStyle(QwtPlot* plot, QString title);
	void timerEvent(QTimerEvent * e);

	void SetNetData();

	int CreateNetCurvs();


	double findMax(double datas[]);
	//重置进程表格内容
	void resetTableWidget(int columnCount);
	//更新进程信息
	void UpdateMonitoredProcessInfo();

	//添加网卡统计信息面板
	void addEthGroup(int index, QString ifName);

	//网络统计信息显示框
	std::vector<QLineEdit*> ethDataLineEdits;

	//性能曲线数组
	PerfCurves perfCurves[MaxCountOfCounter];
	//网络流量曲线
	std::map<std::string, CounterCurve*> netStatCurvs;
	CounterPieMarker *cpuPie, *memoryPie;
	QwtPlot* cpuPlot;
	QwtPlot* memoryPlot;
	QwtPlot* netPlot;
	double timeData[100];
	//工作站管理类
	StationManager* manager;
};

#endif // DETAILDIALOG_H
