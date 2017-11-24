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
	//�鿴ȫ��������Ϣ
	void on_viewALlPushButton_clicked();

	//��Ļ����
	void on_screenCapturePushButton_clicked();
	//����վ״̬�仯
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
	//���ý��̱������
	void resetTableWidget(int columnCount);
	//���½�����Ϣ
	void UpdateMonitoredProcessInfo();

	//�������ͳ����Ϣ���
	void addEthGroup(int index, QString ifName);

	//����ͳ����Ϣ��ʾ��
	std::vector<QLineEdit*> ethDataLineEdits;

	//������������
	PerfCurves perfCurves[MaxCountOfCounter];
	//������������
	std::map<std::string, CounterCurve*> netStatCurvs;
	CounterPieMarker *cpuPie, *memoryPie;
	QwtPlot* cpuPlot;
	QwtPlot* memoryPlot;
	QwtPlot* netPlot;
	double timeData[100];
	//����վ������
	StationManager* manager;
};

#endif // DETAILDIALOG_H
