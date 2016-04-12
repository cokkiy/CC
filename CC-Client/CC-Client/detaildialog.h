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
//���ܼ�����������
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
	//����
	void on_powerOnPushButton_clicked();
	//�ػ�
	void on_powerOffPushButton_clicked();
	//����
	void on_rebootPushButton_clicked();
	//��Ļ����
	void on_screenCapturePushButton_clicked();
	//����վ״̬�仯
	void stationStateChanged(const QObject* obj);

private:
	Ui::DetailDialog ui;
	StationInfo* station;
	void setPlotStyle(QwtPlot* plot, QString title);
	void timerEvent(QTimerEvent * e);

	//���ý��̱������
	void resetTableWidget(int columnCount);
	//���½�����Ϣ
	void UpdateMonitoredProcessInfo();

	//������������
	PerfCurves perfCurves[MaxCountOfCounter];
	CounterPieMarker *cpuPie, *memoryPie;
	QwtPlot* cpuPlot;
	QwtPlot* memoryPlot;
	double timeData[100];
	//����վ������
	StationManager* manager;
};

#endif // DETAILDIALOG_H
