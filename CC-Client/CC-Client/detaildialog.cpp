#include "detaildialog.h"
#include "StationInfo.h"
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include "cpuplot/plotpart.h"
#include <qwt_plot_layout.h>
#include "cpuplot/counterpiemarker.h"
#include <list>
#include "AppControlParameter.h"
#include "stationmanager.h"
#include "screenimagedialog.h"
using namespace std;

DetailDialog::DetailDialog(StationInfo* station, QWidget *parent)
	: QDialog(parent), station(station)
{
	ui.setupUi(this);

	ui.NameLineEdit->setText(station->Name());
	ui.IPLineEdit->setText(station->IP());
	ui.MACLineEdit->setText(station->MAC());

	station->subscribeStateChanged(this, SLOT(stationStateChanged(const QObject*)));

	manager = new StationManager();

	setButtonState();


	setupPlot();

	//初始化时间数据
	for (int i = 0; i < CounterHistoryDataSize; i++)
	{
		timeData[i] = CounterHistoryDataSize - i;
	}

	//总内存大小
	ui.memorySizeLineEdit->setText(QStringLiteral("%1").arg(station->TotalMemory() / 1024 / 1024));

	//启动定时器
	startTimer(1000);  // 1 second
}

void DetailDialog::setButtonState()
{
	if (station->IsRunning())
	{
		ui.powerOnPushButton->setEnabled(false);
		ui.powerOffPushButton->setEnabled(true);
		ui.screenCapturePushButton->setEnabled(true);
		ui.rebootPushButton->setEnabled(true);
	}
	else
	{
		ui.powerOffPushButton->setEnabled(false);
		ui.screenCapturePushButton->setEnabled(false);
		ui.rebootPushButton->setEnabled(false);
		ui.powerOnPushButton->setEnabled(true);
	}
}

void DetailDialog::setupPlot()
{
	cpuPlot = new QwtPlot();
	memoryPlot = new QwtPlot();
	cpuPlot->setMaximumHeight(250);
	memoryPlot->setMaximumHeight(250);
	ui.plotVerticalLayout->addWidget(cpuPlot);
	ui.plotVerticalLayout->addWidget(memoryPlot);
	setPlotStyle(cpuPlot, QStringLiteral("CPU使用情况 [%]"));
	setPlotStyle(memoryPlot, QStringLiteral("内存使用情况 [%]"));


	//显示CPU和内存占用率
	CounterCurve *curve;

	//CPU total
	curve = new CounterCurve("Total");
	curve->setColor(Qt::red);
	curve->attach(cpuPlot);
	curve->setVisible(true);
	perfCurves[0].cpu_curve = curve;

	// memory total
	curve = new CounterCurve("Total");
	curve->setColor(Qt::red);
	curve->attach(memoryPlot);
	curve->setVisible(true);
	perfCurves[0].memory_curve = curve;

	perfCurves[0].name = "Total";

	int zOrder = curve->z() + 1;
	int rgb[] = { 0x00FF40,0x00FFFF,0xFF00FF,0xFF8040,0xFF4000,0x00800,0x00FF80,0x80FFFF,0x0080FF,0xFF80C0,
		0xFF0000,0xFF0000,0x80FF00,0x804040,0x00FF00,0x008080,
		0x004080,0x808000,0x8000FF };
	int index = 1;
	for (auto& proc : station->getAllShouldMonitoredProcessesName())
	{
		//CPU 
		curve = new CounterCurve(QString::fromStdString(proc));
		curve->setColor(QColor::fromRgb(rgb[index - 1]));
		curve->setZ(zOrder);
		curve->attach(cpuPlot);
		curve->setVisible(true);
		perfCurves[index].cpu_curve = curve;

		//Memory
		curve = new CounterCurve(QString::fromStdString(proc));
		curve->setColor(QColor::fromRgb(rgb[index - 1]));
		curve->setZ(zOrder);
		curve->attach(memoryPlot);
		curve->setVisible(true);
		perfCurves[index].memory_curve = curve;

		perfCurves[index].name = proc;
		zOrder++;
		index++;
	}
	// Idle --cpu
	curve = new CounterCurve("Idle");
	curve->setColor(Qt::gray);
	curve->setZ(zOrder);
	curve->attach(cpuPlot);
	curve->setVisible(false);
	perfCurves[index].cpu_curve = curve;

	// idle - memory 
	curve = new CounterCurve("Idle");
	curve->setColor(Qt::gray);
	curve->setZ(zOrder);
	curve->attach(memoryPlot);
	curve->setVisible(false);
	perfCurves[index].memory_curve = curve;

	perfCurves[index].name = "Idle";

	//show pie marker


	cpuPie = new CounterPieMarker(perfCurves, MaxCountOfCounter, true);
	cpuPie->attach(cpuPlot);



	memoryPie = new CounterPieMarker(perfCurves, MaxCountOfCounter, false);
	memoryPie->attach(memoryPlot);

	cpuPlot->updateLayout();
	memoryPlot->updateLayout();
}

DetailDialog::~DetailDialog()
{
	delete cpuPie;
	delete cpuPlot;
	delete memoryPie;
	delete memoryPlot;	
}

void DetailDialog::on_powerOnPushButton_clicked()
{
	manager->powerOn(station);
}

void DetailDialog::on_powerOffPushButton_clicked()
{
	manager->shutdown(station);
}

void DetailDialog::on_rebootPushButton_clicked()
{
	manager->reboot(station);
}

void DetailDialog::on_screenCapturePushButton_clicked()
{
	if (station != NULL&&station->controlProxy != NULL)
	{
		ScreenImageDialog dlg(station);
		dlg.exec();
	}
}

//工作站状态转化
void DetailDialog::stationStateChanged(const QObject* obj)
{
	setButtonState();
}

void DetailDialog::setPlotStyle(QwtPlot* plot, QString title)
{
	plot->setAutoReplot(false);

	QwtPlotCanvas *canvas = new QwtPlotCanvas();
	canvas->setBorderRadius(10);

	plot->setCanvas(canvas);

	plot->plotLayout()->setAlignCanvasToScales(true);

	QwtLegend *legend = new QwtLegend;
	legend->setDefaultItemMode(QwtLegendData::Clickable);
	plot->insertLegend(legend, QwtPlot::LegendPosition::BottomLegend);

	// disable x axis
	plot->enableAxis(QwtPlot::xBottom, false);

	plot->setAxisTitle(QwtPlot::yLeft, title);
	plot->setAxisScale(QwtPlot::yLeft, 0, 100);

	plot->setAxisScale(QwtPlot::xBottom, 0, CounterHistoryDataSize);

	// show grid
	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->attach(plot);
	//show background
	Background *bg = new Background();
	bg->attach(plot);
}

//绘制CPU和内存占用率
void DetailDialog::timerEvent(QTimerEvent *e)
{
	if (station != nullptr)
	{
		for (auto& perfData : station->counterData)
		{
			for (auto& curver : this->perfCurves)
			{
				if (curver.name == perfData.first)
				{
					if (curver.cpu_curve != NULL)
					{
						curver.cpu_curve->setRawSamples(timeData, perfData.second.cpuData, CounterHistoryDataSize);
					}
					if (curver.memory_curve != NULL)
					{
						curver.memory_curve->setRawSamples(timeData, perfData.second.memoryData, CounterHistoryDataSize);
					}
				}
			}
		}
		cpuPlot->replot();
		memoryPlot->replot();

		//更新总体信息
		ui.processCountLineEdit->setText(QStringLiteral("%1").arg(station->ProcCount()));
		ui.CPURateLineEdit->setText(QStringLiteral("%1").arg(station->CPU()));
		ui.memoryRateLineEdit->setText(QStringLiteral("%1").arg(station->Memory() * 100 / (float)station->TotalMemory()));
		//重置进程信息表格
		resetTableWidget(8);
		UpdateMonitoredProcessInfo();
	}
}

void DetailDialog::resetTableWidget(int columnCount)
{
	int rowCount = ui.processTableWidget->rowCount();
	for (int row = 0; row < rowCount; row++)
	{
		for (int coloum = 0; coloum < columnCount; coloum++)
		{
			QTableWidgetItem* item = ui.processTableWidget->takeItem(row, 0);
			if (item != NULL)
				delete item;
		}
	}
	ui.processTableWidget->setRowCount(0);
}

void DetailDialog::UpdateMonitoredProcessInfo()
{
	::CC::AppsRunningState runningStates = station->getAppRunningState();
	for (auto& state : runningStates)
	{
		int row = ui.processTableWidget->rowCount();
		ui.processTableWidget->setRowCount(row + 1);
		QTableWidgetItem *idItem = new QTableWidgetItem(QStringLiteral("%1").arg(state.Process.Id));
		ui.processTableWidget->setItem(row, 0, idItem);
		QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromStdString(state.Process.ProcessName));
		ui.processTableWidget->setItem(row, 1, nameItem);
		QTableWidgetItem *isRunningItem = new QTableWidgetItem(state.isRunning ? QStringLiteral("是") : QStringLiteral("否"));
		ui.processTableWidget->setItem(row, 2, isRunningItem);
		QTableWidgetItem *cpuItem = new QTableWidgetItem(QStringLiteral("%1").arg(state.cpu));
		ui.processTableWidget->setItem(row, 3, cpuItem);
		QTableWidgetItem *memoryItem = new QTableWidgetItem(QStringLiteral("%1").arg(state.currentMemory / (float)station->TotalMemory() * 100));
		ui.processTableWidget->setItem(row, 4, memoryItem);
		QTableWidgetItem *threadCountItem = new QTableWidgetItem(QStringLiteral("%1").arg(state.threadCount));
		ui.processTableWidget->setItem(row, 5, threadCountItem);

		list<CC::AppStartParameter> params = station->getStartAppNames();
		auto finded = find_if(params.begin(), params.end(),
			[&state](const CC::AppStartParameter& para)
		{
			return state.Process.ProcessMonitorName == para.ProcessName;
		});
		if (finded != params.end())
		{
			QString startPath = QString::fromStdString(finded->AppPath);
			QString arguments = QString::fromStdString(finded->Arguments);
			QTableWidgetItem *startPathItem = new QTableWidgetItem(startPath);
			ui.processTableWidget->setItem(row, 6, startPathItem);
			QTableWidgetItem *argumentItem = new QTableWidgetItem(arguments);
			ui.processTableWidget->setItem(row, 7, argumentItem);
		}
	}
}
