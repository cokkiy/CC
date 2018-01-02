#include "detaildialog.h"
#include "StationInfo.h"
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include "IController.h"
#include <Ice/Ice.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include "cpuplot/plotpart.h"
#include <qwt_plot_layout.h>
#include "cpuplot/counterpiemarker.h"
#include <list>
#include "AppControlParameter.h"
#include "stationmanager.h"
#include "screenimagedialog.h"
#include "processdialog.h"
using namespace std;
using namespace CC;

DetailDialog::DetailDialog(StationInfo* station, QWidget *parent)
    : QDialog(parent), station(station)
{
    ui.setupUi(this);
    isClosed = false;
	ui.weatherImageDownloadCheckBox->setChecked(station->getDownloadOption());
    ui.NameLineEdit->setText(station->Name());
    ui.IPLineEdit->setText(station->IP());
    ui.MACLineEdit->setText(station->MAC());

    int index = 1;
    for (auto& ifData : station->getCurrentStatistics().IfStatistics)
    {
        addEthGroup(index, QString::fromStdString(ifData.IfName));
        index++;
    }


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

    //获取版本信息
    if (station != NULL&&station->controlProxy != NULL)
    {
        try
        {
            station->controlProxy->begin_getServerVersion(
                [=](const CC::ServerVersion& serverVersion)
            {
                if (!isClosed)
                {
                    ui.servicesVersionLineEdit->setText(QString::fromStdString(serverVersion.ServicesVersion.ProductVersion));
                    ui.appProxyVersionLineEdit->setText(QString::fromStdString(serverVersion.AppLuncherVersion.ProductVersion));
                }
            },
                [=](const Ice::Exception& ex)
            {
                if (!isClosed)
                {
                    ui.servicesVersionLineEdit->setText(ex.what());
                    ui.appProxyVersionLineEdit->setText(ex.what());
                }
            }
            );
        }
        catch (...)
        {
            if (!isClosed)
            {
                ui.servicesVersionLineEdit->setText(QStringLiteral("无法获取版本信息。"));
                ui.appProxyVersionLineEdit->setText(QStringLiteral("无法获取版本信息。"));
            }
        }
    }

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
    netPlot = new QwtPlot();
    netPlot->setMaximumHeight(250);
    ui.plotVerticalLayout->addWidget(cpuPlot);
    ui.plotVerticalLayout->addWidget(memoryPlot);
    ui.plotVerticalLayout->addWidget(netPlot);
    setPlotStyle(cpuPlot, QStringLiteral("CPU使用情况 [%]"));
    setPlotStyle(memoryPlot, QStringLiteral("内存使用情况 [%]"));
    setPlotStyle(netPlot, QStringLiteral("网络流量[KB/s]"));


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

    zOrder = CreateNetCurvs();

    cpuPlot->updateLayout();
    memoryPlot->updateLayout();
    netPlot->updateLayout();
}

int DetailDialog::CreateNetCurvs()
{
    int zOrder = 0;
    Qt::GlobalColor colors[]{ Qt::green,Qt::darkBlue, Qt::darkRed,Qt::yellow,Qt::darkGreen,Qt::darkYellow,Qt::darkMagenta,Qt::darkCyan };
    for (auto netDtat : station->netStatisticsData)
    {
        if (netStatCurvs.count(netDtat.first) == 0)
        {
            // 网卡名称-total per second
            CounterCurve *curve = new CounterCurve(QString::fromStdString(netDtat.first));
            curve->setColor(colors[zOrder]);
            curve->setZ(zOrder);
            curve->attach(netPlot);
            curve->setVisible(true);
            netStatCurvs[netDtat.first] = curve;
            zOrder++;
        }
    }	return zOrder;
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

void DetailDialog::on_viewALlPushButton_clicked()
{
    ProcessDialog dlg(station);
    dlg.exec();
}


void DetailDialog::on_weatherImageDownloadCheckBox_clicked(bool checked)
{
	IControllerPrx prx = station->controlProxy;
	if (prx != NULL)
	{
		Option::WeatherImageDownloadOption weatherImageDownloadOption = Option::getInstance()->weatherImageDownloadOption;
		CC::WeatherPictureDowlnloadOption ccOption;
		ccOption.Url = weatherImageDownloadOption.Url.toStdString();
		ccOption.UserName = weatherImageDownloadOption.UserName.toStdString();
		ccOption.Password = weatherImageDownloadOption.Password.toStdString();
		ccOption.LastHours = weatherImageDownloadOption.LastHours;
		ccOption.Interval = weatherImageDownloadOption.Interval;
		ccOption.DeletePreviousFiles = weatherImageDownloadOption.DeletePreviousFiles;
		ccOption.DeleteHowHoursAgo = weatherImageDownloadOption.DeleteHowHoursAgo;
		ccOption.SubDirectory = weatherImageDownloadOption.SubDirectory.toStdWString();
		ccOption.SavePathForWindows = weatherImageDownloadOption.SavePathForWindows.toStdWString();
		ccOption.SavePathForLinux = weatherImageDownloadOption.SavePathForLinux.toStdWString();
		if (checked)
		{
			//下载气象云图
			ccOption.Download = true;
		}
		else
		{
			//不下载气象云图
			ccOption.Download = false;
		}

		prx->begin_setWeatherPictureDownloadOption(ccOption, [=, &ccOption]() {
			station->setDownloadOption(ccOption.Download);
		});
	}
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

void DetailDialog::closeEvent(QCloseEvent * event)
{
    isClosed = true;
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

        //绘制网络流量
        double allNetMax = 0;
        for (auto& ifData : station->netStatisticsData)
        {
            double max = findMax(ifData.second);
            allNetMax = max > allNetMax ? max : allNetMax;
            netPlot->setAxisScale(QwtPlot::yLeft, 0, allNetMax + 5);
            netStatCurvs[ifData.first]->setRawSamples(timeData, ifData.second, CounterHistoryDataSize);
        }

        cpuPlot->replot();
        memoryPlot->replot();
        netPlot->replot();

        //更新总体信息
        ui.processCountLineEdit->setText(QStringLiteral("%1").arg(station->ProcCount()));
        ui.CPURateLineEdit->setText(QStringLiteral("%1").arg(station->CPU()));
        ui.memoryRateLineEdit->setText(QStringLiteral("%1").arg(station->Memory() * 100 / (float)station->TotalMemory()));

        //网络数据
        SetNetData();


        //重置进程信息表格
        resetTableWidget(8);
        UpdateMonitoredProcessInfo();
    }
}

void DetailDialog::SetNetData()
{
    CC::Statistics stat = station->getCurrentStatistics();
    ui.datagramsReceivedLineEdit->setText(QString("%1").arg(stat.DatagramsReceived));
    ui.datagramsSentlineEdit->setText(QString("%1").arg(stat.DatagramsSent));
    ui.datagramsDiscardedLineEdit->setText(QString("%1").arg(stat.DatagramsDiscarded));
    ui.datagramsWithErrorsLineEdit->setText(QString("%1").arg(stat.DatagramsWithErrors));
    ui.UDPListenersLineEdit->setText(QString("%1").arg(stat.UDPListeners));
    ui.SegmentsReceivedLineEdit->setText(QString("%1").arg(stat.SegmentsReceived));
    ui.SegmentsSentLineEdit->setText(QString("%1").arg(stat.SegmentsSent));
    ui.ErrorsReceivedLineEdit->setText(QString("%1").arg(stat.ErrorsReceived));
    ui.CurrentConnectionsLineEdit->setText(QString("%1").arg(stat.CurrentConnections));
    ui.ResetConnectionscLineEdit->setText(QString("%1").arg(stat.ResetConnections));
    int ethCount = stat.IfStatistics.size();
    if (ethDataLineEdits.size() == ethCount * 10)
    {
        int index = 0;
        for (auto& ifData : stat.IfStatistics)
        {
            ethDataLineEdits[0 + index * 10]->setText(QString("%1").arg(ifData.BytesReceivedPerSec / 1024, 0, 'f', 3));
            ethDataLineEdits[1 + index * 10]->setText(QString("%1").arg(ifData.BytesSentedPerSec / 1024, 0, 'f', 3));
            ethDataLineEdits[2 + index * 10]->setText(QString("%1").arg(ifData.TotalBytesPerSec / 1024, 0, 'f', 3));
            ethDataLineEdits[3 + index * 10]->setText(QString("%1").arg(ifData.BytesReceived / 1024.0 / 1024.0, 0, 'f', 3));
            ethDataLineEdits[4 + index * 10]->setText(QString("%1").arg(ifData.BytesSented / 1024.0 / 1024.0, 0, 'f', 3));
            ethDataLineEdits[5 + index * 10]->setText(QString("%1").arg(ifData.BytesTotal / 1024.0 / 1024.0, 0, 'f', 3));
            ethDataLineEdits[6 + index * 10]->setText(QString("%1").arg(ifData.UnicastPacketReceived));
            ethDataLineEdits[7 + index * 10]->setText(QString("%1").arg(ifData.UnicastPacketSented));
            ethDataLineEdits[8 + index * 10]->setText(QString("%1").arg(ifData.MulticastPacketReceived));
            ethDataLineEdits[9 + index * 10]->setText(QString("%1").arg(ifData.MulticastPacketSented));

            index++;
        }
    }
}

double DetailDialog::findMax(double datas[])
{
    double max = 0;
    for (qint64 i = 0; i < CounterHistoryDataSize; i++)
    {
        max = max >= datas[i] ? max : datas[i];
    }

    return max;
}

void DetailDialog::resetTableWidget(int columnCount)
{
    int rowCount = ui.processTableWidget->rowCount();
    for (int row = 0; row < rowCount; row++)
    {
        for (int coloum = 0; coloum < columnCount; coloum++)
        {
            QTableWidgetItem* item = ui.processTableWidget->takeItem(row, coloum);
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

void DetailDialog::addEthGroup(int index, QString ifName)
{
    QGroupBox* ethGroupBox = new QGroupBox(ui.processFrame);
    ethGroupBox->setTitle(QStringLiteral("%1(%2) 统计数据").arg(ifName).arg(station->IP(ifName)));
    QVBoxLayout* groupVerticalLayout = new QVBoxLayout(ethGroupBox);
    groupVerticalLayout->setSpacing(6);
    groupVerticalLayout->setContentsMargins(11, 11, 11, 11);
    QHBoxLayout* horizontalLayout_line1 = new QHBoxLayout();
    horizontalLayout_line1->setSpacing(6);

    QLabel* label_1 = new QLabel(ethGroupBox);

    label_1->setText(QStringLiteral("接收(KB/s)"));
    horizontalLayout_line1->addWidget(label_1);

    QLineEdit* bytesReceivedPerSecLineEdit = new QLineEdit(ethGroupBox);
    bytesReceivedPerSecLineEdit->setObjectName(QStringLiteral("bytesReceivedPerSecLineEdit"));
    ethDataLineEdits.push_back(bytesReceivedPerSecLineEdit);
    horizontalLayout_line1->addWidget(bytesReceivedPerSecLineEdit);

    QLabel* label_2 = new QLabel(ethGroupBox);

    label_2->setText(QStringLiteral("发送(KB/s)"));
    horizontalLayout_line1->addWidget(label_2);

    QLineEdit* bytesSentPerSeclineEdit = new QLineEdit(ethGroupBox);
    ethDataLineEdits.push_back(bytesSentPerSeclineEdit);
    horizontalLayout_line1->addWidget(bytesSentPerSeclineEdit);

    QLabel* label_3 = new QLabel(ethGroupBox);

    label_3->setText(QStringLiteral("总流量(KB/s)"));
    horizontalLayout_line1->addWidget(label_3);

    QLineEdit* bytesTotalPerSecLineEdit = new QLineEdit(ethGroupBox);
    ethDataLineEdits.push_back(bytesTotalPerSecLineEdit);
    horizontalLayout_line1->addWidget(bytesTotalPerSecLineEdit);


    groupVerticalLayout->addLayout(horizontalLayout_line1);

    QHBoxLayout* horizontalLayout_line2 = new QHBoxLayout();
    horizontalLayout_line2->setSpacing(6);
    horizontalLayout_line2->setObjectName(QStringLiteral("horizontalLayout_6"));
    QLabel* label_4 = new QLabel(ethGroupBox);

    label_4->setText(QStringLiteral("已接收(MB)"));
    horizontalLayout_line2->addWidget(label_4);

    QLineEdit* bytesReceivedLineEdit = new QLineEdit(ethGroupBox);
    ethDataLineEdits.push_back(bytesReceivedLineEdit);
    horizontalLayout_line2->addWidget(bytesReceivedLineEdit);

    QLabel* label_5 = new QLabel(ethGroupBox);
    label_5->setObjectName(QStringLiteral("label_16"));
    label_5->setText(QStringLiteral("已发送(MB)"));
    horizontalLayout_line2->addWidget(label_5);

    QLineEdit* bytesSentedLineEdit = new QLineEdit(ethGroupBox);
    ethDataLineEdits.push_back(bytesSentedLineEdit);
    bytesSentedLineEdit->setObjectName(QStringLiteral("bytesSentPerSeclineEdit_3"));

    horizontalLayout_line2->addWidget(bytesSentedLineEdit);

    QLabel* label_6 = new QLabel(ethGroupBox);
    label_6->setObjectName(QStringLiteral("label_17"));
    label_6->setText(QStringLiteral("总量(MB)"));
    horizontalLayout_line2->addWidget(label_6);

    QLineEdit* bytesTotalLineEdit = new QLineEdit(ethGroupBox);
    ethDataLineEdits.push_back(bytesTotalLineEdit);
    bytesTotalLineEdit->setObjectName(QStringLiteral("bytesTotalPerSecLineEdit_3"));

    horizontalLayout_line2->addWidget(bytesTotalLineEdit);


    groupVerticalLayout->addLayout(horizontalLayout_line2);

    QHBoxLayout* horizontalLayout_line3 = new QHBoxLayout();
    horizontalLayout_line3->setSpacing(6);

    QLabel* label_7 = new QLabel(ethGroupBox);

    label_7->setText(QStringLiteral("单播收（包）"));
    horizontalLayout_line3->addWidget(label_7);

    QLineEdit* unicastReceivedLineEdit = new QLineEdit(ethGroupBox);
    ethDataLineEdits.push_back(unicastReceivedLineEdit);
    horizontalLayout_line3->addWidget(unicastReceivedLineEdit);

    QLabel* label_8 = new QLabel(ethGroupBox);
    label_8->setObjectName(QStringLiteral("label_14"));
    label_8->setText(QStringLiteral("单播发(包)"));
    horizontalLayout_line3->addWidget(label_8);

    QLineEdit* unicastSentLineEdit = new QLineEdit(ethGroupBox);
    ethDataLineEdits.push_back(unicastSentLineEdit);

    horizontalLayout_line3->addWidget(unicastSentLineEdit);


    groupVerticalLayout->addLayout(horizontalLayout_line3);

    QHBoxLayout* horizontalLayout_line4 = new QHBoxLayout();
    horizontalLayout_line4->setSpacing(6);
    //horizontalLayout_line4->setObjectName(QStringLiteral("horizontalLayout_8"));
    QLabel* label_9 = new QLabel(ethGroupBox);
    //label_19->setObjectName(QStringLiteral("label_19"));
    label_9->setText(QStringLiteral("组播收（包）"));
    horizontalLayout_line4->addWidget(label_9);

    QLineEdit* multiCastReceivedLineEdit = new QLineEdit(ethGroupBox);
    ethDataLineEdits.push_back(multiCastReceivedLineEdit);
    horizontalLayout_line4->addWidget(multiCastReceivedLineEdit);

    QLabel* label_10 = new QLabel(ethGroupBox);
    label_10->setObjectName(QStringLiteral("label_20"));
    label_10->setText(QStringLiteral("组播发(包)"));
    horizontalLayout_line4->addWidget(label_10);

    QLineEdit* multiCastSentedLineEdit = new QLineEdit(ethGroupBox);
    ethDataLineEdits.push_back(multiCastSentedLineEdit);
    horizontalLayout_line4->addWidget(multiCastSentedLineEdit);
    groupVerticalLayout->addLayout(horizontalLayout_line4);

    // add group to process panel
    ui.processVerticalLayout->insertWidget(index, ethGroupBox);
}
