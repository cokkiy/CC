﻿#include "mainwindow.h"
#include "ProgressBarDelegate.h"
#include "StationList.h"
#include "StationStateReceiver.h"
#include "aboutdialog.h"
#include "autonetcfgdialog.h"
#include "batchcapturedialog.h"
#include "defaultappprocdialog.h"
#include "detaildialog.h"
#include "editstationdialog.h"
#include "filterbuilddialog.h"
#include "floatingmenu.h"
#include "monitor.h"
#include "netconfigdlg.h"
#include "option.h"
#include "qevent.h"
#include "qpagenumdialog.h"
#include "qtdispatcher.h"
#include "recvfiledialog.h"
#include "screenimagedialog.h"
#include "sendfiledialog.h"
#include "setintervaldialog.h"
#include "stationmanager.h"
#include "ui_mainwindow.h"
#include "updatemanager.h"
#include "upgradestationservicedlg.h"
#include "weatherimageoptiondlg.h"
#include "xdocument.h"
#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QItemSelectionModel>
#include <QMenu>
#include <QMessageBox>
#include <QSignalMapper>
#include <QStatusBar>
#include <QTableWidget>
#include <counterpiemarker.h>
#include <plotpart.h>
#include <qwt_legend.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include "packetgraph/captureprogressdlg.h"
#include "packetgraph/packetanalyzewindow.h"
#include "sysupgrade/sysupgradewizard.h"
#include "createvnidialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  setAttribute(Qt::WA_NoMousePropagation);

  createClearPageSubSystemSelectMenu();

  //设置CPU和内存使用记录样式
  QwtPlot *cpuPlot = ui->qwtPlotCPU;
  setPlotStyle(cpuPlot, QStringLiteral("CPU使用情况 [%]"));
  QwtPlot *memPlot = ui->qwtPlotMemory;
  setPlotStyle(memPlot, QStringLiteral("内存使用情况 [%]"));
  QwtPlot *netPlot = ui->qwtPlotNetStat;
  setPlotStyle(netPlot, QStringLiteral("网络流量 [KB/s]"));

  //绑定菜单显示事件
  connect(ui->menuOperation, SIGNAL(aboutToShow()), this,
          SLOT(operationMenuToShow()));
  connect(ui->menuFileTranslation, &QMenu::aboutToShow, this,
          &MainWindow::fileTransMenuToShow);
  //隐藏tableView,只显示listView
  ui->tableView->setVisible(false);
  tableHeader = ui->tableView->horizontalHeader();
  tableHeader->setSortIndicator(1, Qt::AscendingOrder); //默认, IP 正序
  //绑定点击排序
  connect(tableHeader, SIGNAL(sectionClicked(int)), this,
          SLOT(sortByColumn(int)));
  //工作站文件名
  filePath = QDir::homePath() + QStringLiteral("/.CC-Client/");
  fileName = filePath + QStringLiteral("指显工作站信息.xml");
  //加载用户选项
  option.Load();
  //初始化工作站信息列表
  pStationList = new StationList();

  //初始化时间数据
  for (int i = 0; i < CounterHistoryDataSize; i++) {
    timeData[i] = CounterHistoryDataSize - i;
  }

  // 初始化加载线程
  StationsLoader *loader = new StationsLoader();
  loader->moveToThread(&ldstation_thread);
  connect(&ldstation_thread, &QThread::finished, loader, &QObject::deleteLater);
  connect(this, &MainWindow::load, loader, &StationsLoader::load);
  connect(loader, &StationsLoader::loaded, this, &MainWindow::stationsLoaded);
  connect(ui->menuManager, &QMenu::aboutToShow, this,
          &MainWindow::ManagerMenuToShow);
  ldstation_thread.start();
}

MainWindow::~MainWindow() {
  delete ui;
  if (pStationList != nullptr)
    delete pStationList;
  if (pTableModel != nullptr)
    delete pTableModel;
  if (monitor != nullptr)
    delete monitor;
}

void MainWindow::ManagerMenuToShow() {
  //打开指定计算机电源
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (selectedJustOne(selectedIndexs)) {
    ui->actionNetBlock->setEnabled(true);
    auto station = pStationList->atCurrent(selectedIndexs.first().row());
    if (station->state().isBlocked()) {
      ui->actionNetBlock->setText(QStringLiteral("允许访问网络"));
      QIcon itemIcon(":/Icons/icons8-cloud-done-50.png");
      ui->actionNetBlock->setIcon(itemIcon);
      ui->actionNetBlock->setToolTip(QStringLiteral("允许该设备访问网络"));
      ui->actionNetBlock->setStatusTip(
          QStringLiteral("点击允许该设备访问网络"));
    } else {
      ui->actionNetBlock->setText(QStringLiteral("禁止访问网络"));
      QIcon itemIcon(":/Icons/icons8-cloud-cross-50.png");
      ui->actionNetBlock->setIcon(itemIcon);
      ui->actionNetBlock->setToolTip(QStringLiteral("禁止该设备访问网络"));
      ui->actionNetBlock->setStatusTip(
          QStringLiteral("点击禁止该设备访问网络"));
    }
  } else {
    ui->actionNetBlock->setEnabled(false);
  }
}

void MainWindow::on_actionPowerOn_triggered() {
  if (QMessageBox::warning(
          this, QStringLiteral("启动工作站确认"),
          QStringLiteral("确实要启动选定的工作站吗？已经启动的不受影响。"),
          QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
    //打开指定计算机电源
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager *manager = new StationManager(pStationList, selectedIndexs);
    manager->powerOn();
  }
}

void MainWindow::on_actionPoweroff_triggered() {
  if (QMessageBox::warning(this, QStringLiteral("关闭工作站确认"),
                           QStringLiteral("确实要关闭选定的工作站吗？"),
                           QMessageBox::Yes,
                           QMessageBox::No) == QMessageBox::Yes) {
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager *manager = new StationManager(pStationList, selectedIndexs);
    //关机
    manager->shutdown();
  }
}

void MainWindow::on_actionReboot_triggered() {
  if (QMessageBox::warning(this, QStringLiteral("重启工作站确认"),
                           QStringLiteral("确实要重启选定的工作站吗？"),
                           QMessageBox::Yes,
                           QMessageBox::No) == QMessageBox::Yes) {
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager *manager = new StationManager(pStationList, selectedIndexs);
    //重启
    manager->reboot();
  }
}

void MainWindow::on_actionStartApp_triggered() {
  if (QMessageBox::warning(
          this, QStringLiteral("启动程序确认"),
          QStringLiteral(
              "确实要启动选定设备上的程序吗？程序已经运行的工作站不受影响。"),
          QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager *manager = new StationManager(pStationList, selectedIndexs);
    //启动程序
    manager->startApp();
  }
}

void MainWindow::on_actionRestartApp_triggered() {
  if (QMessageBox::warning(this, QStringLiteral("重启工作站程序确认"),
                           QStringLiteral("确实要重启选定设备上的程序吗？"),
                           QMessageBox::Yes,
                           QMessageBox::No) == QMessageBox::Yes) {
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager *manager = new StationManager(pStationList, selectedIndexs);
    //启动程序
    manager->restartApp();
  }
}

void MainWindow::on_actionNetBlock_triggered() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (selectedJustOne(selectedIndexs)) {
    auto station = pStationList->atCurrent(selectedIndexs.first().row());
    if (station->state().isBlocked()) {
      if (QMessageBox::question(this, QStringLiteral("允许访问网络确认"),
                                QStringLiteral("确实要允许该设备访问网络吗？"),
                                QMessageBox::Yes,
                                QMessageBox::No) == QMessageBox::Yes) {
        QModelIndexList selectedIndexs = getSelectedIndexs();
        //创建工作站管理类
        StationManager *manager =
            new StationManager(pStationList, selectedIndexs);
        //启动程序
        manager->block(false);
      }
    } else {
      if (QMessageBox::question(this, QStringLiteral("阻止访问网络确认"),
                                QStringLiteral("确实要阻止该设备访问网络吗？"),
                                QMessageBox::Yes,
                                QMessageBox::No) == QMessageBox::Yes) {
        QModelIndexList selectedIndexs = getSelectedIndexs();
        //创建工作站管理类
        StationManager *manager =
            new StationManager(pStationList, selectedIndexs);
        //启动程序
        manager->block(true);
      }
    }
  }
}

void MainWindow::on_actionNetTrafficViewer_triggered() {
  CaptureProgressDlg dlg(this, Qt::Popup);
  dlg.exec();
  PacketAnalyzeWindow window(
      this, Qt::WindowMaximizeButtonHint | Qt::WindowTitleHint |
                Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
  window.exec();
}

void MainWindow::on_actionSystemUpgrade_triggered()
{
  SysUpgradeWizard wizard(this);
  wizard.exec();
}

void MainWindow::on_actionCreateVirtualNI_triggered()
{
  CreateVNIDialog dlg(this);
  dlg.exec();
}

void MainWindow::on_actionNetConfig_triggered() {
  NetConfigDlg dlg;
  dlg.exec();
}

void MainWindow::on_actionExitApp_triggered() {
  if (QMessageBox::warning(this, QStringLiteral("退出工作站程序确认"),
                           QStringLiteral("确实要退出选定工作站的程序吗？"),
                           QMessageBox::Yes,
                           QMessageBox::No) == QMessageBox::Yes) {
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager *manager = new StationManager(pStationList, selectedIndexs);
    //退出程序
    manager->exitApp();
  }
}

void MainWindow::on_actionAutoNetSetting_triggered() {
  AutoNetCfgDialog dlg;
  dlg.exec();
}

void MainWindow::on_actionAllPower_On_triggered() {
  if (QMessageBox::warning(
          this, QStringLiteral("工作站启动确认"),
          QStringLiteral("确实要启动所有工作站吗？已经启动工作站不受影响。"),
          QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager *manager = new StationManager(pStationList, selectedIndexs);
    //打开全部计算机
    manager->powerOnAll();
  }
}
//启动全部工作站程序
void MainWindow::on_actionStartAllApp_triggered() {
  if (QMessageBox::warning(this, QStringLiteral("启动程序确认"),
                           QStringLiteral("确实要启动所有工作站上的程序吗？程"
                                          "序已经运行的工作站不受影响。"),
                           QMessageBox::Yes,
                           QMessageBox::No) == QMessageBox::Yes) {
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager *manager = new StationManager(pStationList, selectedIndexs);
    manager->startAllApp();
  }
}

//全部重启
void MainWindow::on_actionRebootAll_triggered() {
  if (QMessageBox::warning(this, QStringLiteral("重启工作站确认"),
                           QStringLiteral("确实要重启所有工作站吗？"),
                           QMessageBox::Yes,
                           QMessageBox::No) == QMessageBox::Yes) {
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager *manager = new StationManager(pStationList, selectedIndexs);
    manager->rebootAll();
  }
}
//全部重启应用
void MainWindow::on_actionRestartAll_triggered() {
  if (QMessageBox::warning(this, QStringLiteral("重启程序确认"),
                           QStringLiteral("确实要重启所有工作站上的软件吗？"),
                           QMessageBox::Yes,
                           QMessageBox::No) == QMessageBox::Yes) {
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager *manager = new StationManager(pStationList, selectedIndexs);
    manager->restartAllApp();
  }
}

//全部退出程序
void MainWindow::on_actionExitAllZXApp_triggered() {
  if (QMessageBox::warning(this, QStringLiteral("退出程序确认"),
                           QStringLiteral("确实要关闭所有工作站上的软件吗？"),
                           QMessageBox::Yes,
                           QMessageBox::No) == QMessageBox::Yes) {
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager *manager = new StationManager(pStationList, selectedIndexs);
    manager->exitAllApp();
  }
}
//全部关机
void MainWindow::on_actionAll_Shutdown_triggered() {
  if (QMessageBox::warning(this, QStringLiteral("关闭工作站确认"),
                           QStringLiteral("确实要关闭所有工作站吗？"),
                           QMessageBox::Yes,
                           QMessageBox::No) == QMessageBox::Yes) {
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager *manager = new StationManager(pStationList, selectedIndexs);
    manager->shutdownAll();
  }
}

//工作站信息加载完毕
void MainWindow::stationsLoaded(StationList *pStations, bool success) {
  if (success) {
    ui->statusBar->showMessage(QStringLiteral("工作站信息加载完毕。"), 5000);
  } else {
    ui->statusBar->showMessage(
        QStringLiteral("无法加载工作站信息，%1无法打开。").arg(fileName), 3000);
  }
  pStations->sort(StationList::IP);
  //设置模型和视图
  pTableModel = new StationTableModel(pStations);
  //订阅状态变化
  pStations->subscribeAllStationsStateChangedEvent(
      this, SLOT(stationStateChanged(const QObject *)));
  pStations->subscribeAllStationsChangedEvent(
      this, SLOT(stationChanged(const QObject *)));
  pStations->subscribeStationAddedEvent(this,
                                        SLOT(onStationAdded(StationInfo *)));
  pStations->subscribeListChangedEvent(this, SLOT(onStationListChanged()));
  ui->tableView->setModel(pTableModel);
  //设置显示代理
  ProgressBarDelegate *barDelegate = new ProgressBarDelegate(this);
  ui->tableView->setItemDelegateForColumn(3, barDelegate);
  ui->tableView->setItemDelegateForColumn(4, barDelegate);

  ui->listView->setModel(pTableModel);
  //绑定鼠标右键事件,显示悬浮式菜单
  connect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(showFloatingMenu(const QPoint &)));
  connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(showFloatingMenu(const QPoint &)));
  //绑定当前选择发生变化事件
  connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this,
          SLOT(currentStationChanged(const QModelIndex &)));
  connect(ui->listView, SIGNAL(clicked(const QModelIndex &)), this,
          SLOT(currentStationChanged(const QModelIndex &)));

  //默认列表模式
  DetailMode();
  //启动状态监视
  monitor = new Monitor(option.Interval);
  monitor->setStationList(pStations);
  monitor->start(QThread::NormalPriority);
  if (iceInitSuccess) {
    // updateManager = new UpdateManager(communicator, this);
    // connect(updateManager, &UpdateManager::UpdatingProgressReport, this,
    // &MainWindow::on_UpdatingProgressReported);
    StationStateReceiverPtr =
        new StationStateReceiver(NULL, communicator, pStations);
    Ice::Identity
        stateReceiver; // = communicator->stringToIdentity("stateReceiver");
    stateReceiver.name = "stateReceiver";
    adapter->add(StationStateReceiverPtr, stateReceiver);
    adapter->activate();
  }
}

//点击标题栏排序
void MainWindow::sortByColumn(int cloumnIndex) {}

//显示悬浮式菜单
void MainWindow::showFloatingMenu(const QPoint &pos) {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (selectedIndexs.count() == 0)
    return;

  //悬浮式菜单
  FloatingMenu *menu = new FloatingMenu(this);
  menu->setSize(250);
  //根据选择的工作站添加按钮到右键菜单
  addButtons(menu);
  //设置背景色和外边颜色
  QPen border(QBrush(QColor(255, 255, 0)), 3, Qt::DotLine);
  QRadialGradient gradient(0, 0, 120, 0, 0);
  gradient.setColorAt(0, QColor::fromRgbF(1, 1, 0, 0.8));
  gradient.setColorAt(1, QColor::fromRgbF(0.3, 0.5, 0, 0.5));
  QBrush brush(gradient);
  menu->setBackground(brush);
  menu->setBorder(border);
  //显示菜单
  menu->show(this->cursor().pos().x(), this->cursor().pos().y());
}

// 操作菜单即将显示,根据选择情况控制菜单可用性
void MainWindow::operationMenuToShow() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (selectedIndexs.isEmpty()) {
    //没有选择
    for (auto action : ui->menuOperation->actions()) {
      action->setEnabled(false);
    }
  } else if (selectedJustOne(selectedIndexs)) {
    //只选择了一个
    for (auto action : ui->menuOperation->actions()) {
      //先禁用全部
      action->setEnabled(false);
    }
    auto station = pStationList->atCurrent(selectedIndexs.first().row());
    if (station->state() == StationInfo::Unknown) {
      ui->actionPowerOn->setEnabled(true);
    } else if (station->IsRunning()) {
      ui->actionReboot->setEnabled(true);
      ui->actionPoweroff->setEnabled(true);
      //屏幕截图
      ui->actionScreenCapture->setEnabled(true);
      //如果(部分)应用已经启动
      if (station->state() == StationInfo::AppStarted ||
          station->state() == StationInfo::SomeAppNotRunning) {
        ui->actionRestartApp->setEnabled(true);
        ui->actionExitApp->setEnabled(true);
        if (station->state() == StationInfo::SomeAppNotRunning) {
          //有些程序没有启动
          ui->actionStartApp->setEnabled(true);
        }
      } else if (station->state() == StationInfo::AppNotRunning ||
                 station->state() == StationInfo::AppStartFailure) {
        //应用程序没有运行
        ui->actionStartApp->setEnabled(true);
      }
    }
    //详细信息菜单起用
    ui->actionViewStationDetail->setEnabled(true);
    //编辑按钮起作用
    ui->actionEdit->setEnabled(true);
    //删除按钮起作用
    ui->actionRemove->setEnabled(true);
  } else {
    //选择了多个
    for (auto action : ui->menuOperation->actions()) {
      action->setEnabled(true);
    }
    //编辑按钮禁用
    ui->actionEdit->setEnabled(false);
    //详细信息菜单禁用
    ui->actionViewStationDetail->setEnabled(false);
    //屏幕截图
    ui->actionScreenCapture->setEnabled(false);
  }
  //新增按钮总是起作用
  ui->actionNewStation->setEnabled(true);
  //添加程序和进程按钮总是起作用
  ui->actionAddCtrlProgram->setEnabled(true);
}

//文件传输菜单即将显示
void MainWindow::fileTransMenuToShow() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (selectedIndexs.isEmpty()) {
    //没有选择
    ui->actionSendSelection->setEnabled(false);
    ui->actionReceiveSelection->setEnabled(false);
  } else {
    ui->actionSendSelection->setEnabled(true);
    ui->actionReceiveSelection->setEnabled(true);
  }
}

//工作站状态发生变化
void MainWindow::stationStateChanged(const QObject *pObject) {
  StationInfo *pStation = (StationInfo *)pObject;
  QTime now = QTime::currentTime();
  QListWidgetItem *item =
      new QListWidgetItem(QStringLiteral("%1:%2:%3 %4 %5")
                              .arg(now.hour(), 2, 10, QLatin1Char('0'))
                              .arg(now.minute(), 2, 10, QLatin1Char('0'))
                              .arg(now.second(), 2, 10, QLatin1Char('0'))
                              .arg(pStation->toString())
                              .arg(pStation->state().toString()));
  if (pStation->state() == StationInfo::Normal) {
    item->setForeground(Qt::black);
  } else if (pStation->state() == StationInfo::Warning) {
    item->setForeground(QColor(131, 66, 2));
  } else // Error
  {
    item->setForeground(Qt::red);
  }
  ui->msgListWidget->addItem(item);
  ui->msgListWidget->setCurrentItem(
      ui->msgListWidget->item(ui->msgListWidget->count() - 1));
}

//工作站被添加处理函数
void MainWindow::onStationAdded(StationInfo *addedStation) {
  addedStation->subscribeStateChanged(
      this, SLOT(stationStateChanged(const QObject *)));
  addedStation->subscribeStationChanged(this,
                                        SLOT(stationChanged(const QObject *)));
}

//清空消息记录
void MainWindow::clearMessage() { ui->msgListWidget->clear(); }
//拷贝选择的消息
void MainWindow::copyMessage() {
  if (ui->msgListWidget->currentItem() != NULL) {
    auto item = ui->msgListWidget->currentIndex();
    QGuiApplication::clipboard()->setText(item.data().toString());
  }
}
//拷贝全部消息
void MainWindow::copyAllMessage() {
  QString msg;
  for (int i = 0; i < ui->msgListWidget->count(); i++) {
    msg += QStringLiteral("%1\r\n").arg(
        ui->msgListWidget->item(i)->data(0).toString());
  }
  QGuiApplication::clipboard()->setText(msg);
}

//工作站列表发生变化
void MainWindow::onStationListChanged() {
  QDir dir(filePath);
  if (!dir.exists()) {
    dir.mkdir(filePath);
  }
  pStationList->saveToFile(fileName);
}

//工作站发生变化,用户编辑了工作站属性
void MainWindow::stationChanged(const QObject *pStation) {
  QDir dir(filePath);
  if (!dir.exists()) {
    dir.mkdir(filePath);
  }
  pStationList->saveToFile(fileName);

  // 更新工作站监视程序列表
  StationInfo *s =
      const_cast<StationInfo *>(dynamic_cast<const StationInfo *>(pStation));
  if (s->controlProxy != NULL) {
    //设置工作站监视进程列表
    s->controlProxy->begin_setWatchingApp(
        s->getAllShouldMonitoredProcessesName(), []() {});
  }
}

//当前选择的工作站发生变化
void MainWindow::currentStationChanged(const QModelIndex &current) {
  for (auto &pc : perfCurves) {
    if (pc.cpu_curve != NULL) {
      pc.cpu_curve->detach();
      delete pc.cpu_curve;
      pc.cpu_curve = NULL;
    }
    if (pc.memory_curve != NULL) {
      pc.memory_curve->detach();
      delete pc.memory_curve;
      pc.memory_curve = NULL;
    }
  }

  for (auto &netCurve : netStatCurvs) {
    netCurve.second->detach();
    delete netCurve.second;
  }
  netStatCurvs.clear();

  currentStation = pStationList->atCurrent(current.row());
  //显示CPU和内存占用率，网络流量
  QwtPlot *cpuPlot = ui->qwtPlotCPU;
  QwtPlot *memoryPlot = ui->qwtPlotMemory;

  CounterCurve *curve;

  // CPU total
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
  int rgb[] = {0x00FF40, 0x00FFFF, 0xFF00FF, 0xFF8040, 0xFF4000,
               0x00800,  0x00FF80, 0x80FFFF, 0x0080FF, 0xFF80C0,
               0xFF0000, 0xFF0000, 0x80FF00, 0x804040, 0x00FF00,
               0x008080, 0x004080, 0x808000, 0x8000FF};
  int index = 1;
  for (auto &proc : currentStation->getAllShouldMonitoredProcessesName()) {
    // CPU
    curve = new CounterCurve(QString::fromStdString(proc));
    curve->setColor(QColor::fromRgb(rgb[index - 1]));
    curve->setZ(zOrder);
    curve->attach(cpuPlot);
    curve->setVisible(true);
    perfCurves[index].cpu_curve = curve;

    // Memory
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

  // show pie marker
  if (cpuPie != nullptr)
    delete cpuPie;

  cpuPie = new CounterPieMarker(perfCurves, MaxCountOfCounter, true);
  cpuPie->attach(cpuPlot);

  if (memoryPie != nullptr)
    delete memoryPie;

  memoryPie = new CounterPieMarker(perfCurves, MaxCountOfCounter, false);
  memoryPie->attach(memoryPlot);

  ui->qwtPlotCPU->updateLayout();
  ui->qwtPlotMemory->updateLayout();

  //网络流量图
  CreateNetCurver();
}

void MainWindow::CreateNetCurver() {
  int zOrder = 0;
  QwtPlot *netPlot = ui->qwtPlotNetStat;
  zOrder = 0;
  Qt::GlobalColor colors[]{Qt::green,       Qt::darkBlue,  Qt::darkRed,
                           Qt::yellow,      Qt::darkGreen, Qt::darkYellow,
                           Qt::darkMagenta, Qt::darkCyan};
  for (auto netDtat : currentStation->netStatisticsData) {
    if (netStatCurvs.count(netDtat.first) == 0) {
      // 网卡名称-total per second
      CounterCurve *curve =
          new CounterCurve(QString::fromStdString(netDtat.first));
      curve->setColor(colors[zOrder]);
      curve->setZ(zOrder);
      curve->attach(netPlot);
      curve->setVisible(true);
      netStatCurvs[netDtat.first] = curve;
      zOrder++;
    }
  }

  ui->qwtPlotNetStat->updateLayout();
}

//工作站更新状态发生变化
void MainWindow::on_UpdatingProgressReported(int percent, QString message) {
  QString text = QStringLiteral("已完成%1\\%，%2").arg(percent).arg(message);
  QListWidgetItem *item = new QListWidgetItem(text);
  ui->msgListWidget->addItem(item);
  ui->msgListWidget->setCurrentItem(
      ui->msgListWidget->item(ui->msgListWidget->count() - 1));
}

/*!
重载show函数,实现加载配置信息并显示窗口
@return void
作者：cokkiy（张立民)
创建时间：2015/11/03 15:43:42
*/
void MainWindow::show() {
  QMainWindow::show();
  ui->statusBar->showMessage(QStringLiteral("开始加载工作站信息..."), 0);
  QString dir = QApplication::applicationDirPath();
  //初始化ICE框架,创建监听通道
  try {
    int argc = 0;
    Ice::InitializationData initData;
    initData.properties = Ice::createProperties();
    //加载配置文件
    initData.properties->load("Config.Server");
    //装载Qt版本Dispatcher
    dispatcher = new QtDispatcher(false);
    initData.dispatcher = dispatcher;
    communicator = Ice::initialize(argc, 0, initData);
    adapter = communicator->createObjectAdapter("StateReceiver");
    //初始化成功
    iceInitSuccess = true;
  } catch (const IceUtil::Exception &ex) {
    iceInitSuccess = false;
    ostringstream ostr;
    ostr << ex;
    QString s = QString::fromStdString(ostr.str());
    QMessageBox::information(
        NULL, QStringLiteral("警告"),
        QStringLiteral("创建工作站状态监视通道失败：%1").arg(s));
  }

  //开始加载工作站配置文件
  emit load(fileName, pStationList);

  if (option.IsFirstTimeRun) {
    DefaultAppProcDialog dlg(option);
    if (dlg.exec() == QDialog::Accepted) {
      option.Save();
    }
  }

  //启动定时器
  timerId = startTimer(1000); // 1 second
}

//关闭程序前提示确认
void MainWindow::closeEvent(QCloseEvent *event) {
  //关闭系统
  if (QMessageBox::question(this, QStringLiteral("集中监控程序"),
                            QStringLiteral("确定要退出集中监控程序吗?"),
                            QMessageBox::Ok,
                            QMessageBox::Cancel) == QMessageBox::Ok) {
    try {
      monitor->Stop();

      if (timerId != 0) {
        killTimer(timerId);
      }

      if (dispatcher != NULL) {
        dispatcher->stop();
      }
      communicator->shutdown();
      communicator->destroy();
    } catch (const IceUtil::Exception &) {
    }

    QThread::sleep(1);
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::on_sortComboBox_currentIndexChanged(int index) {
  switch (index) {
  case 0: // IP
    pStationList->sort(StationList::IP);
    break;
  case 1: // IP DESC
    pStationList->sort(StationList::IP_DESC);
    break;
  case 2: // State
    pStationList->sort(StationList::State);
    break;
  case 3: // State DESC
    pStationList->sort(StationList::State_DESC);
    break;
  case 4: // Name
    pStationList->sort(StationList::Name);
    break;
  case 5: // NAME DESC
    pStationList->sort(StationList::Name_DESC);
    break;
  default:
    break;
  }
}

//显示方式发生变化
void MainWindow::on_displayModeComboBox_currentIndexChanged(QString index) {
  if (index.compare(tr("Detail")) == 0) {
    //表格模式
    DetailMode();
  } else {
    //列表模式
    ui->listView->setVisible(true);
    ui->tableView->setVisible(false);
    if (index.compare(tr("Small Icons")) == 0) {
      //小图标
      smallIcomMode();
    } else if (index.compare(tr("Larger Icons")) == 0) {
      //大图标
      pTableModel->setDisplayMode(StationTableModel::LargerIcons);
      ui->listView->setUniformItemSizes(true);
      ui->listView->setIconSize({64, 64});
      ui->listView->setViewMode(QListView::ViewMode::IconMode);
    } else if (index.compare(tr("List")) == 0) {
      //列表
      pTableModel->setDisplayMode(StationTableModel::List);
      ui->listView->setUniformItemSizes(false);
      ui->listView->setIconSize({32, 32});
      ui->listView->setViewMode(QListView::ViewMode::ListMode);
    }
  }
}

void MainWindow::DetailMode() {
  pTableModel->setDisplayMode(StationTableModel::Details);
  ui->listView->setVisible(false);
  ui->tableView->setVisible(true);
}

//小图标模式
void MainWindow::smallIcomMode() {
  pTableModel->setDisplayMode(StationTableModel::SmallIcons);
  ui->listView->setUniformItemSizes(true);
  ui->listView->setIconSize({32, 32});
  ui->listView->setViewMode(QListView::ViewMode::IconMode);
}

//点击过滤按钮
void MainWindow::on_filterToolButton_clicked() {
  FilterBuildDialog buildDlg;
  buildDlg.setGeometry(this->cursor().pos().x(), this->cursor().pos().y() + 20,
                       buildDlg.width(), buildDlg.height());
  if (buildDlg.exec() == QDialog::Accepted) {
    FilterOperations operations = buildDlg.getFilter();
    ui->filterLineEdit->setText(operations.toString());
  }
}
//查看CPU占用率
void MainWindow::on_actionViewCPU_triggered(bool checked) {
  if (checked) {
    ui->qwtPlotCPU->setVisible(true);
  } else {
    ui->qwtPlotCPU->setVisible(false);
  }
}

void MainWindow::on_actionViewMemory_triggered(bool checked) {
  if (checked) {
    ui->qwtPlotMemory->setVisible(true);
  } else {
    ui->qwtPlotMemory->setVisible(false);
  }
}

void MainWindow::on_actionViewDetail_triggered(bool checked) {
  if (checked) {
    ui->detailFrame->setVisible(true);
  } else {
    ui->detailFrame->setVisible(false);
  }
}

//是否显示消息
void MainWindow::on_actionViewMessage_triggered(bool checked) {
  if (checked) {
    ui->msgListWidget->setVisible(true);
  } else {
    ui->msgListWidget->setVisible(false);
  }
}
//消息列表右键菜单
void MainWindow::on_msgListWidget_customContextMenuRequested(
    const QPoint &point) {
  FloatingMenu *menu = new FloatingMenu(this);
  menu->setSize(200);
  if (ui->msgListWidget->count() > 0) {
    menu->addButton(":/Icons/52design.com_3d_08.png", QStringLiteral("清空"),
                    this, SLOT(clearMessage()));
    menu->addButton(":/Icons/PP15.jpg", QStringLiteral("复制"), this,
                    SLOT(copyMessage()));
    menu->addButton(":/Icons/52design.com_jdxt_045.png",
                    QStringLiteral("全部复制"), this, SLOT(copyAllMessage()));
  } else {
    menu->addButton(":/Icons/52design.com_3d_08.png", QStringLiteral("清空"));
    menu->addButton(":/Icons/PP15.jpg", QStringLiteral("复制"));
    menu->addButton(":/Icons/52design.com_jdxt_045.png",
                    QStringLiteral("全部复制"));
  }

  menu->show(this->cursor().pos().x(), this->cursor().pos().y());
}

// 设置监视间隔
void MainWindow::on_actionSetInterval_triggered() {
  if (iceInitSuccess) {
    SetIntervalDialog *dlg = new SetIntervalDialog(option.Interval);
    dlg->setGeometry(this->cursor().pos().x(), this->cursor().pos().y(),
                     dlg->width(), dlg->height());
    if (dlg->exec() == QDialog::Accepted) {
      option.Interval = dlg->Interval();
      if (monitor != nullptr) {
        monitor->setInterval(option.Interval);
      }
      option.Save();
      //设置监视间隔
      StationManager *manager = new StationManager(pStationList);
      manager->setInterval(option.Interval);
    }
    delete dlg;
  } else {
    QMessageBox::information(
        this, QStringLiteral("警告"),
        QStringLiteral("初始化通信框架失败,无法设置监视间隔."));
  }
}

//自动更新工作站列表
void MainWindow::on_actionAllowAutoRefreshList_triggered(bool checked) {
  if (StationStateReceiverPtr != nullptr) {
    StationStateReceiverPtr->setAutoRefreshStationList(checked);
  }
}

//新加工作站
void MainWindow::on_actionNewStation_triggered() {
  StationInfo *pStation = new StationInfo;
  pStation->addStartApps(option.StartApps);
  pStation->addStandAloneMonitorProcesses(option.MonitorProcesses);
  EditStationDialog dlg(pStation, EditStationDialog::New);
  if (dlg.exec() == QDialog::Accepted) {
    pStationList->push(*pStation);
  }
}

//编辑工作站
void MainWindow::on_actionEdit_triggered() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (!selectedIndexs.isEmpty()) {
    auto station = pStationList->atCurrent(selectedIndexs.first().row());
    EditStationDialog dlg(station, EditStationDialog::Edit);
    dlg.exec();
  }
}

//添加控制的程序和监视的进程
void MainWindow::on_actionAddCtrlProgram_triggered() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (selectedIndexs.isEmpty()) {
    if (QMessageBox::question(
            NULL, QStringLiteral("编辑确认"),
            QStringLiteral("要向所有工作站添加控制的程序和监视的进程吗?"),
            QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) {
      return;
    }
  }

  DefaultAppProcDialog dlg(option,
                           QStringLiteral("添加要控制的程序和监视的进程"));
  if (dlg.exec() == QDialog::Accepted) {
    if (selectedIndexs.isEmpty()) {
      for (auto iter = pStationList->begin(); iter != pStationList->end();
           iter++) {
        iter->addStandAloneMonitorProcesses(option.MonitorProcesses);
        iter->addStartApps(option.StartApps);
      }
    } else {
      for (auto index : selectedIndexs) {
        if (index.column() == 0) {
          StationInfo *s = pStationList->atCurrent(index.row());
          s->addStandAloneMonitorProcesses(option.MonitorProcesses);
          s->addStartApps(option.StartApps);
        }
      }
    }

    //保存修改
    pStationList->saveToFile(fileName);
  }
}

//删除工作站
void MainWindow::on_actionRemove_triggered() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (QMessageBox::question(NULL, QStringLiteral("删除确认"),
                            QStringLiteral("要删除（所有)选定的工作站吗?"),
                            QMessageBox::Yes,
                            QMessageBox::No) == QMessageBox::Yes) {
    for (auto index : selectedIndexs) {
      if (index.column() == 0) {
        StationInfo *s = pStationList->atCurrent(index.row());
        if (currentStation == s) {
          currentStation = nullptr;
        }
        pStationList->remove(s);
      }
    }
  }
}

//设置新建工作站默认监视进程和启动程序
void MainWindow::on_actionSetDefaultAppAndProc_triggered() {
  DefaultAppProcDialog dlg(option);
  if (dlg.exec() == QDialog::Accepted) {
    option.Save();
  }
}

//设置气象云图下载选项
void MainWindow::on_actionSetWeatherDownloadOption_triggered() {
  WeatherImageOptionDlg dlg(&option.weatherImageDownloadOption);
  if (dlg.exec() == QDialog::Accepted) {
    option.Save();
    //设置监视间隔
    StationManager *manager = new StationManager(pStationList);
    manager->setWeatherImageDownloadOption(option.weatherImageDownloadOption);
  }
}

//查看工作站信息信息
void MainWindow::on_actionViewStationDetail_triggered() {
  QModelIndexList selectedIndexs = getSelectedIndexs();

  if (selectedIndexs.isEmpty())
    return;

  auto station = pStationList->atCurrent(selectedIndexs.first().row());
  if (station != NULL) {
    //显示工作站详细信息对话框
    DetailDialog *dlg = new DetailDialog(station);
    dlg->exec();
    delete dlg;
  }
}

//发送文件到选定工作站
void MainWindow::on_actionSendSelection_triggered() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (selectedIndexs.isEmpty())
    return;

  SendFileDialog *dlg =
      new SendFileDialog(pStationList, selectedIndexs, false, communicator);
  dlg->exec();
}
//从选定工作站接收文件
void MainWindow::on_actionReceiveSelection_triggered() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (selectedIndexs.isEmpty())
    return;
  RecvFileDialog *dlg =
      new RecvFileDialog(pStationList, selectedIndexs, false, communicator);
  dlg->exec();
}
//发送文件到全部工作站
void MainWindow::on_actionSendAll_triggered() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  SendFileDialog *dlg =
      new SendFileDialog(pStationList, selectedIndexs, true, communicator);
  dlg->exec();
}

//接收文件从全部工作站
void MainWindow::on_actionReceiveAll_triggered() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  RecvFileDialog *dlg =
      new RecvFileDialog(pStationList, selectedIndexs, true, communicator);
  dlg->exec();
}

//过滤
void MainWindow::on_filterLineEdit_returnPressed() {
  QString filter = ui->filterLineEdit->text();
  pTableModel->beginFilter();
  pStationList->filter(filter);
  pTableModel->endFilter();
}

//屏幕快照
void MainWindow::on_actionScreenCapture_triggered() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (selectedIndexs.isEmpty())
    return;
  auto station = pStationList->atCurrent(selectedIndexs.first().row());
  if (station != NULL && station->controlProxy != NULL) {
    ScreenImageDialog dlg(station);
    dlg.exec();
  }
}
//批量屏幕快照
void MainWindow::on_actionBatchScreenCapture_triggered() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (selectedIndexs.isEmpty())
    return;
  BatchCaptureDialog dlg(pStationList, selectedIndexs, this);
  dlg.exec();
}

//显示关于对话框
void MainWindow::on_actionAbout_triggered() {
  AboutDialog dlg;
  dlg.exec();
}

//升级中控服务
void MainWindow::on_actionUpdateStationService_triggered() {
  bool allStations = false;
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (selectedIndexs.isEmpty()) {
    allStations = true;
  }
  UpgradeStationServiceDlg *dlg = new UpgradeStationServiceDlg(
      pStationList, selectedIndexs, allStations, communicator);
  dlg->exec();
  delete dlg;
}

//升级中控代理
void MainWindow::on_actionUpgradeAppProxy_triggered() {
  bool allStations = false;
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (selectedIndexs.isEmpty()) {
    allStations = true;
  }

  UpgradeStationServiceDlg *dlg = new UpgradeStationServiceDlg(
      pStationList, selectedIndexs, allStations, communicator, false);
  dlg->exec();
  delete dlg;
}

//清屏
void MainWindow::on_clearPagePushButton_clicked() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  // int subSystem = ui->clearPagePushButton->property("SubSystem").toInt();
  //创建工作站管理类
  //   StationManager *manager = new StationManager(pStationList,
  //   selectedIndexs); manager->clearPage(subSystem);
}

void MainWindow::changeClearPageSubSystem(QAction *action) {
  //   ui->clearPagePushButton->setText(
  //       QStringLiteral("清\"%1\"").arg(action->text()));
  //   ui->clearPagePushButton->setToolTip(
  //       QStringLiteral("清除选定工作站\"%1\"").arg(action->text()));
  //   ui->clearPagePushButton->setStatusTip(
  //       QStringLiteral("清除选定工作站\"%1\"").arg(action->text()));
  //   ui->clearPagePushButton->setProperty("SubSystem",
  //                                        action->property("SubSystem"));
}

//全屏
void MainWindow::on_fullDcreenPushButton_clicked() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  //创建工作站管理类
  StationManager *manager = new StationManager(pStationList, selectedIndexs);
  manager->fullScreen();
}

//实时模式
void MainWindow::on_realtimePushButton_clicked() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  //创建工作站管理类
  StationManager *manager = new StationManager(pStationList, selectedIndexs);
  manager->realTime();
}

//前一页
void MainWindow::on_prevPagePushButton_clicked() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  //创建工作站管理类
  StationManager *manager = new StationManager(pStationList, selectedIndexs);
  manager->prevPage();
}

//下一页
void MainWindow::on_nextPushButton_clicked() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  //创建工作站管理类
  StationManager *manager = new StationManager(pStationList, selectedIndexs);
  manager->nextPage();
}

//打开指定画面
void MainWindow::on_openSpecPagePushButton_clicked() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (!selectedIndexs.empty()) {
    QPageNumDialog dlg;
    if (dlg.exec() == QDialog::Accepted) {
      StationManager *manager =
          new StationManager(pStationList, selectedIndexs);
      manager->openPage(dlg.value());
    }
  } else {
    QMessageBox::warning(this, QStringLiteral("警告"),
                         QStringLiteral("必须至少选择一个工作站。"));
  }
}

// 关闭工作站上的指显软件（通过发送关闭命令实现）
void MainWindow::on_closeComDisplayPushButton_clicked() {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  if (!selectedIndexs.empty()) {
    if (QMessageBox::question(
            this, QStringLiteral("关闭确认"),
            QStringLiteral("确实要选定工作站上的关闭指显软件吗？"),
            QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
      StationManager *manager =
          new StationManager(pStationList, selectedIndexs);
      manager->closeComDisplay();
    }
  } else {
    QMessageBox::warning(this, QStringLiteral("警告"),
                         QStringLiteral("必须至少选择一个工作站。"));
  }
}

//升级工作站
void MainWindow::on_actionUpdateStation_triggered() {
  if (QMessageBox::question(
          this, QStringLiteral("确认升级"),
          QStringLiteral("升级结束后将会自动重启被升级的系统，确认升级吗？"),
          QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
    if (updateManager != NULL) {
      updateManager->start();
    }
  }
}

//添加按钮到右键菜单
void MainWindow::addButtons(FloatingMenu *menu) {
  QModelIndexList selectedIndexs = getSelectedIndexs();
  //创建工作站管理类
  StationManager *manager =
      new StationManager(pStationList, selectedIndexs, this);
  if (!selectedJustOne(selectedIndexs)) {
    //扩大menu大小
    menu->setSize(300);
    //选择了多个
    menu->addButton(":/Icons/52design.com_jingying_108.png",
                    QStringLiteral("开机"), this,
                    SLOT(on_actionPowerOn_triggered()));
    // menu->addButton(":/Icons/200969173136504.png", QStringLiteral("重启"),
    // manager, SLOT(reboot()));
    menu->addButton(":/Icons/52design.com_jingying_098.png",
                    QStringLiteral("关机"), this,
                    SLOT(on_actionPoweroff_triggered()));
    menu->addButton(":/Icons/52design.com_jingying_059.png",
                    QStringLiteral("启动任务"), this,
                    SLOT(on_actionStartApp_triggered()));
    // menu->addButton(":/Icons/2009724113238761.png",
    // QStringLiteral("重启程序"), manager, SLOT(restartApp()));
    menu->addButton(":/Icons/52design.com_alth_171.png",
                    QStringLiteral("关闭任务"), this,
                    SLOT(on_actionExitApp_triggered()));
    menu->addButton(":/Icons/receiveFile.png", QStringLiteral("下载数据"), this,
                    SLOT(on_actionReceiveSelection_triggered()));
    menu->addButton(":/Icons/sendFile.png", QStringLiteral("上传任务"), this,
                    SLOT(on_actionSendSelection_triggered()));
    menu->addButton(":/Icons/capture.png", QStringLiteral("批量截屏"), this,
                    SLOT(on_actionBatchScreenCapture_triggered()));
  } else {
    //只选择了一个,则根据状态显示菜单
    auto station = pStationList->atCurrent(selectedIndexs.first().row());
    if (!station->IsRunning()) {
      //未开机
      menu->addButton(":/Icons/52design.com_jingying_108.png",
                      QStringLiteral("开机"), this,
                      SLOT(on_actionPowerOn_triggered()));
      menu->addButton(":/Icons/52design.com_2006ball_74.png",
                      QStringLiteral("编辑"), this,
                      SLOT(on_actionEdit_triggered()));
      menu->addButton(":/Icons/png-0652.png", QStringLiteral("删除"), this,
                      SLOT(on_actionRemove_triggered()));
    } else {
      //已开机
      menu->addButton(":/Icons/200969173136504.png", QStringLiteral("重启"),
                      this, SLOT(on_actionReboot_triggered()));
      menu->addButton(":/Icons/52design.com_jingying_098.png",
                      QStringLiteral("关机"), this,
                      SLOT(on_actionPoweroff_triggered()));
      if (station->state() == StationInfo::AppStarted ||
          station->state() == StationInfo::SomeAppNotRunning) {
        //扩大menu大小
        menu->setSize(320);
        menu->addButton(":/Icons/2009724113238761.png",
                        QStringLiteral("重启任务"), this,
                        SLOT(on_actionRestartApp_triggered()));
        menu->addButton(":/Icons/52design.com_alth_171.png",
                        QStringLiteral("退出任务"), this,
                        SLOT(on_actionExitApp_triggered()));

        if (station->state() == StationInfo::SomeAppNotRunning) {
          //有些程序没有启动
          menu->addButton(":/Icons/52design.com_jingying_059.png",
                          QStringLiteral("启动任务"), this,
                          SLOT(on_actionStartApp_triggered()));
          //扩大menu
          menu->setSize(320);
        }
      } else if (station->state() == StationInfo::AppNotRunning ||
                 station->state() == StationInfo::AppStartFailure) {
        //扩大menu大小
        menu->setSize(300);
        //应用程序没有运行
        menu->addButton(":/Icons/52design.com_jingying_059.png",
                        QStringLiteral("启动任务"), this,
                        SLOT(on_actionStartApp_triggered()));
      }

      menu->addButton(":/Icons/receiveFile.png", QStringLiteral("下载数据"),
                      this, SLOT(on_actionReceiveSelection_triggered()));
      menu->addButton(":/Icons/sendFile.png", QStringLiteral("上传任务"), this,
                      SLOT(on_actionSendSelection_triggered()));
      menu->addButton(":/Icons/screencapture.png", QStringLiteral("屏幕快照"),
                      this, SLOT(on_actionScreenCapture_triggered()));
    }
    menu->addButton(":/Icons/52design.com_file_036.png",
                    QStringLiteral("详细信息"), this,
                    SLOT(on_actionViewStationDetail_triggered()));
  }
}

//获取当前选择的工作站索引
QModelIndexList MainWindow::getSelectedIndexs() {
  QModelIndexList selectedIndexs;
  if (ui->listView->isVisible()) {
    //当前是listView
    QItemSelectionModel *selection = ui->listView->selectionModel();
    selectedIndexs = selection->selectedIndexes();
  } else {
    //当前是 tableView
    QItemSelectionModel *selection = ui->tableView->selectionModel();
    selectedIndexs = selection->selectedIndexes();
  }

  return selectedIndexs;
}

//是否只选择了一个工作站
bool MainWindow::selectedJustOne(QModelIndexList selectedIndexs) {
  if (ui->listView->isVisible())
    return selectedIndexs.count() == 1;
  //如果是表格,则做进一步判断
  if (selectedIndexs.isEmpty())
    return false;
  int row = selectedIndexs.first().row();
  for (auto index : selectedIndexs) {
    if (row != index.row())
      return false;
  }
  return true;
}

void MainWindow::setPlotStyle(QwtPlot *plot, QString title) {
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
  // show background
  Background *bg = new Background();
  bg->attach(plot);
}

//绘制CPU和内存占用率
void MainWindow::timerEvent(QTimerEvent *e) {
  if (currentStation != nullptr) {
    for (auto &perfData : currentStation->counterData) {
      for (auto &curver : this->perfCurves) {
        if (curver.name == perfData.first) {
          if (curver.cpu_curve != NULL) {
            curver.cpu_curve->setRawSamples(timeData, perfData.second.cpuData,
                                            CounterHistoryDataSize);
          }
          if (curver.memory_curve != NULL) {
            curver.memory_curve->setRawSamples(
                timeData, perfData.second.memoryData, CounterHistoryDataSize);
          }
        }
      }
    }

    //绘制网络流量
    double allNetMax = 0;
    for (auto &ifData : currentStation->netStatisticsData) {
      if (netStatCurvs.count(ifData.first) == 0) {
        CreateNetCurver();
      }
      double max = findMax(ifData.second);
      allNetMax = max > allNetMax ? max : allNetMax;
      ui->qwtPlotNetStat->setAxisScale(QwtPlot::yLeft, 0, allNetMax + 5);
      netStatCurvs[ifData.first]->setRawSamples(timeData, ifData.second,
                                                CounterHistoryDataSize);
    }
    ui->qwtPlotNetStat->replot();
    ui->qwtPlotCPU->replot();
    ui->qwtPlotMemory->replot();

    //显示线程数和进程数
    ui->procCountlineEdit->setText(
        QStringLiteral("%1").arg(currentStation->ProcCount()));
  }
}

double MainWindow::findMax(double datas[]) {
  double max = 0;
  for (qint64 i = 0; i < CounterHistoryDataSize; i++) {
    max = max >= datas[i] ? max : datas[i];
  }

  return max;
}

void MainWindow::createClearPageSubSystemSelectMenu() {
  QMenu *clearPageSystemMenu = new QMenu(this);
  QAction *f1Action = new QAction(clearPageSystemMenu);
  f1Action->setText(QStringLiteral("所有画面"));
  QAction *f2Action = new QAction(clearPageSystemMenu);
  f2Action->setText(QStringLiteral("C3I画面"));
  QAction *f3Action = new QAction(clearPageSystemMenu);
  f3Action->setText(QStringLiteral("东风画面"));
  QAction *f4Action = new QAction(clearPageSystemMenu);
  f4Action->setText(QStringLiteral("库尔勒画面"));
  QAction *f5Action = new QAction(clearPageSystemMenu);
  f5Action->setText(QStringLiteral("自定义画面1（F5）"));
  QAction *f6Action = new QAction(clearPageSystemMenu);
  f6Action->setText(QStringLiteral("自定义画面2（F6）"));
  QAction *f7Action = new QAction(clearPageSystemMenu);
  f7Action->setText(QStringLiteral("自定义画面3（F7）"));
  QAction *f8Action = new QAction(clearPageSystemMenu);
  f8Action->setText(QStringLiteral("自定义画面4（F8）"));

  QAction *actions[] = {f1Action, f2Action, f3Action, f4Action,
                        f5Action, f6Action, f7Action, f8Action};
  QActionGroup *actionGroup = new QActionGroup(this);
  for (int i = 0; i < 8; i++) {
    actionGroup->addAction(actions[i]);
    actions[i]->setCheckable(true);
    actions[i]->setProperty("SubSystem", i);
    clearPageSystemMenu->addAction(actions[i]);
  }
  connect(actionGroup, &QActionGroup::triggered, this,
          &MainWindow::changeClearPageSubSystem);
  f1Action->setChecked(true);
  //   ui->clearPagePushButton->setText(QStringLiteral("清\"所有画面\""));
  //   ui->clearPagePushButton->setToolTip(
  //       QStringLiteral("清除选定工作站\"所有画面\""));
  //   ui->clearPagePushButton->setStatusTip(
  //       QStringLiteral("清除选定工作站\"所有画面\""));
  //   ui->clearPagePushButton->setProperty("SubSystem",
  //                                        f1Action->property("SubSystem"));
  //   ui->clearPageSelectButton->setMenu(clearPageSystemMenu);
}

//加载工作站信息
void StationsLoader::load(const QString filename, StationList *pStations) {
  XDocument doc;
  if (doc.Load(filename)) {
    auto elements =
        doc.getChild().getChildrenByName(QStringLiteral("指显工作站"));
    for (XElement el : elements) {
      StationInfo station;
      station.setName(el.getChildValue(QStringLiteral("名称")));
      for (XElement &niElemet : el.getChildrenByName(QStringLiteral("网卡"))) {
        QString mac = niElemet.getChildValue(QStringLiteral("MAC"));
        QStringList ipList;
        for (XElement &ipElement :
             niElemet.getChildrenByName(QStringLiteral("IP"))) {
          ipList.push_back(ipElement.Value);
        }
        NetworkInterface ni(mac, ipList);
        station.NetworkIntefaces.push_back(ni);
      }
      for (XElement &appElement :
           el.getChildrenByName(QStringLiteral("启动程序/程序"))) {
        station.addStartApp(
            appElement.getChildValue(QStringLiteral("路径")),
            appElement.getChildValue(QStringLiteral("参数")),
            appElement.getChildValue(QStringLiteral("进程名")),
            appElement.getChildBoolValue(QStringLiteral("允许多实例")));
      }
      for (XElement &procElement :
           el.getChildrenByName(QStringLiteral("监视进程/进程"))) {
        station.addStandAloneMonitorProcess(procElement.Value);
      }
      pStations->push(station);
    }
    emit loaded(pStations, true);
  } else {
    emit loaded(pStations, false);
  }
}
