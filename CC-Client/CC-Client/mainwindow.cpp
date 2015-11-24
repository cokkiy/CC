#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filterbuilddialog.h"
#include <QMessageBox>
#include <selectcomputerdialog.h>
#include <QApplication>
#include <QDir>
#include "xdocument.h"
#include <QStatusBar>
#include <QTableWidget>
#include "qevent.h"
#include "StationList.h"
#include "floatingmenu.h"
#include <QItemSelectionModel>
#include "stationmanager.h"
#include "monitor.h"
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //绑定菜单显示事件
    connect(ui->menuOperation, SIGNAL(aboutToShow()), this, SLOT(operationMenuToShow()));
    //隐藏tableView,只显示listView
    ui->tableView->setVisible(false);
    tableHeader = ui->tableView->horizontalHeader();
    tableHeader->setSortIndicator(1, Qt::AscendingOrder); //默认, IP 正序
    //绑定点击排序
    connect(tableHeader, SIGNAL(sectionClicked(int)), this, SLOT(sortByColumn(int)));
    //初始化工作站信息列表 
    pStationList = new StationList();
    // 初始化加载线程
    ConfigLoader* loader = new ConfigLoader();
    loader->moveToThread(&ldconf_thread);
    connect(&ldconf_thread, &QThread::finished, loader, &QObject::deleteLater);
    connect(this, &MainWindow::load, loader, &ConfigLoader::load);
    connect(loader, &ConfigLoader::loaded, this, &MainWindow::configLoaded);
    connect(loader, &ConfigLoader::fail, this, &MainWindow::configLoadFailed);
    ldconf_thread.start();
}

MainWindow::~MainWindow()
{
    delete ui;
    if(pStationList!=nullptr)
        delete pStationList;
    if (pTableModel != nullptr)
        delete pTableModel;
}

void MainWindow::on_actionPower_On_triggered()
{
    //打开指定计算机电源
    selectComputerDialog dlg(this);
    dlg.exec();
}

void MainWindow::on_actionAll_Power_On_triggered()
{
    //打开全部计算机
}

//工作站信息加载完毕
void MainWindow::configLoaded(StationList* pStations)
{
    ui->statusBar->showMessage(QString::fromLocal8Bit("工作站信息加载完毕。"),5000);
    //设置模型和视图
    pTableModel = new StationTableModel(pStations);
    //订阅状态变化
    pStations->subscribeAllStationsStateChangedEvent(this, SLOT(stationStateChanged(const QObject*)));
    ui->tableView->setModel(pTableModel);
    ui->listView->setModel(pTableModel);
    //绑定鼠标进入事件,显示悬浮式菜单
    connect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showFloatingMenu(const QPoint &)));
    connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showFloatingMenu(const QPoint &)));
    //默认小图标模式
    smallIcomMode();
    //启动状态监视
    Monitor* monitor = new Monitor();
    monitor->setStationList(pStations);
    monitor->start(QThread::NormalPriority);
}
//工作站信息加载失败
void MainWindow::configLoadFailed(const QString& fileName)
{
    ui->statusBar->showMessage(QString::fromLocal8Bit("无法加载工作站信息，%1无法打开。").arg(fileName), 3000);
}

//点击标题栏排序
void MainWindow::sortByColumn(int cloumnIndex)
{
}

//显示悬浮式菜单
void MainWindow::showFloatingMenu(const QPoint & pos)
{
    //悬浮式菜单    
    FloatingMenu* menu = new FloatingMenu(this);
    menu->setSize(200);
    //根据选择的工作站添加按钮到右键菜单
    addButtons(menu);
    //设置背景色和外边颜色
    QPen border(QBrush(QColor(255,255,0)), 3,Qt::DotLine); 
    QRadialGradient gradient(0, 0, 120, 0, 0);
    gradient.setColorAt(0, QColor::fromRgbF(1,1,0,0.8));
    gradient.setColorAt(1, QColor::fromRgbF(0.3, 0.5, 0, 0.5));
    QBrush brush(gradient);
    menu->setBackground(brush);
    menu->setBorder(border);
    //显示菜单
    menu->show(this->cursor().pos().x(), this->cursor().pos().y());
}

// 操作菜单即将显示,根据选择情况控制菜单可用性
void MainWindow::operationMenuToShow()
{
    QModelIndexList selectedIndexs = getSelectedIndexs();
    if (selectedIndexs.isEmpty())
    {
        //没有选择
        for (auto action : ui->menuOperation->actions())
        {
            action->setEnabled(false);
        }
    }
    else if(selectedJustOne(selectedIndexs))
    {
        //只选择了一个
        for (auto action : ui->menuOperation->actions())
        {
            //先禁用全部
            action->setEnabled(false);
        }
        auto station = pStationList->atCurrent(selectedIndexs.first().row());
        if (station->state()==StationInfo::Unkonown)
        {
            ui->actionPowerOn->setEnabled(true);
        }
        else if(station->StationIsRunning())
        {
            ui->actionReboot->setEnabled(true);
            ui->actionShutdown->setEnabled(true);
            if (station->AppIsRunning())
            {
                ui->actionRestartApp->setEnabled(true);
                ui->actionExitApp->setEnabled(true);
            }
            else
            {
                ui->actionStartApp->setEnabled(true);
            }
        } 
        else if(!station->inExecutingState())
        {
            ui->actionPowerOn->setEnabled(true);
        }
    }
    else
    {
        //选择了多个
        for (auto action : ui->menuOperation->actions())
        {
            action->setEnabled(true);
        }
    }
}

//工作站状态发生变化
void MainWindow::stationStateChanged(const QObject* pObject)
{
    StationInfo* pStation = (StationInfo*)pObject;
    switch (pStation->state())
    {
    case StationInfo::Unkonown:
        break;
    case  StationInfo::Powering:
        ui->msgListWidget->addItem(QStringLiteral("向%1（%2)发送开机命令...").arg(pStation->IP).arg(pStation->name));
        break;
    case StationInfo::PowerOnFailure:
        ui->msgListWidget->addItem(QStringLiteral("%1(%2)开机失败.").arg(pStation->IP).arg(pStation->name));
        break;
    default:
        break;
    }
    ui->msgListWidget->setCurrentItem(ui->msgListWidget->item(ui->msgListWidget->count() - 1));
}
//清空消息记录
void MainWindow::clearMessage()
{
    ui->msgListWidget->clear();
}
//拷贝选择的消息
void MainWindow::copyMessage()
{
    if (ui->msgListWidget->currentItem() != NULL)
    {
        auto item = ui->msgListWidget->currentIndex();
        QGuiApplication::clipboard()->setText(item.data().toString());
    }
}
//拷贝全部消息
void MainWindow::copyAllMessage()
{
    QString msg;
    for (int i = 0; i < ui->msgListWidget->count(); i++)
    {
        msg += QStringLiteral("%1\r\n").arg(ui->msgListWidget->item(i)->data(0).toString());
    }
    QGuiApplication::clipboard()->setText(msg);
}

/*!
重载show函数,实现加载配置信息并显示窗口
@return void
作者：cokkiy（张立民)
创建时间：2015/11/03 15:43:42
*/
void MainWindow::show()
{
    QMainWindow::show();
    ui->statusBar->showMessage(QString::fromLocal8Bit("开始加载工作站信息..."), 0);
    QString dir = QApplication::applicationDirPath();
    QString fileName = dir + QString::fromLocal8Bit("/../config/指显工作站信息.xml");
    emit load(fileName, pStationList); //开始加载配置文件
}

//关闭程序前提示确认
void MainWindow::closeEvent(QCloseEvent * event)
{
    //关闭系统
    if (QMessageBox::information(this, QString::fromLocal8Bit("集中监控程序"), 
        QString::fromLocal8Bit("确定要退出集中监控程序吗?"),
        QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }    
}

void MainWindow::on_sortComboBox_currentIndexChanged(int index)
{

}

//显示方式发生变化
void MainWindow::on_displayModeComboBox_currentIndexChanged(QString index)
{
    if (index.compare(tr("Detail")) == 0)
    {
        //表格模式
        pTableModel->setDisplayMode(StationTableModel::Details);
        ui->listView->setVisible(false);
        ui->tableView->setVisible(true);
    }
    else
    {
        //列表模式
        ui->listView->setVisible(true);
        ui->tableView->setVisible(false);
        if (index.compare(tr("Small Icons")) == 0)
        {
            //小图标
            smallIcomMode();
        }
        else if (index.compare(tr("Larger Icons")) == 0)
        {
            //大图标
            pTableModel->setDisplayMode(StationTableModel::LargerIcons);
            ui->listView->setUniformItemSizes(true);
            ui->listView->setIconSize({ 64,64 });
            ui->listView->setViewMode(QListView::ViewMode::IconMode);
        }
        else if (index.compare(tr("List")) == 0)
        {
            //列表
            pTableModel->setDisplayMode(StationTableModel::List);
            ui->listView->setUniformItemSizes(false);
            ui->listView->setIconSize({ 32,32 });
            ui->listView->setViewMode(QListView::ViewMode::ListMode);
        }
    }    
}

//小图标模式（默认模式)
void MainWindow::smallIcomMode()
{
    pTableModel->setDisplayMode(StationTableModel::SmallIcons);
    ui->listView->setUniformItemSizes(true);
    ui->listView->setIconSize({ 32,32 });
    ui->listView->setViewMode(QListView::ViewMode::IconMode);
}

//点击过滤按钮
void MainWindow::on_filterToolButton_clicked()
{
    FilterBuildDialog buildDlg;
    buildDlg.setGeometry(this->cursor().pos().x(), this->cursor().pos().y()+20, buildDlg.width(), buildDlg.height());
    if (buildDlg.exec() == QDialog::Accepted)
    {
        FilterOperations operations = buildDlg.getFilter();
        ui->filterLineEdit->setText(operations.toString());
    }
}
//查看CPU占用率
void MainWindow::on_actionViewCPU_triggered(bool checked)
{
    if (checked)
    {
        ui->qwtPlotCPU->setVisible(true);
    }
    else
    {
        ui->qwtPlotCPU->setVisible(false);
    }
}

void MainWindow::on_actionViewMemory_triggered(bool checked)
{
    if (checked)
    {
        ui->qwtPlotMemory->setVisible(true);
    }
    else
    {
        ui->qwtPlotMemory->setVisible(false);
    }
}

void MainWindow::on_actionViewDetail_triggered(bool checked)
{
    if (checked)
    {
        ui->detailFrame->setVisible(true);
    }
    else
    {
        ui->detailFrame->setVisible(false);
    }    
}

//是否显示消息
void MainWindow::on_actionViewMessage_triggered(bool checked)
{
    if (checked)
    {
        ui->msgListWidget->setVisible(true);
    }
    else
    {
        ui->msgListWidget->setVisible(false);
    }
}
//消息列表右键菜单
void MainWindow::on_msgListWidget_customContextMenuRequested(const QPoint & point)
{
    FloatingMenu* menu = new FloatingMenu(this);
    menu->setSize(150);
    if (ui->msgListWidget->count() > 0)
    {
        menu->addButton(":/Icons/52design.com_3d_08.png", QStringLiteral("清空"), this, SLOT(clearMessage()));
        menu->addButton(":/Icons/PP15.jpg", QStringLiteral("复制"), this, SLOT(copyMessage()));
        menu->addButton(":/Icons/52design.com_jdxt_045.png", QStringLiteral("全部复制"), this, SLOT(copyAllMessage()));
    }
    else
    {
        menu->addButton(":/Icons/52design.com_3d_08.png", QStringLiteral("清空"));
        menu->addButton(":/Icons/PP15.jpg", QStringLiteral("复制"));
        menu->addButton(":/Icons/52design.com_jdxt_045.png", QStringLiteral("全部复制"));
    }

    menu->show(this->cursor().pos().x(), this->cursor().pos().y());
}

//添加按钮到右键菜单
void MainWindow::addButtons(FloatingMenu* menu)
{
    QModelIndexList selectedIndexs = getSelectedIndexs();
    //创建工作站管理类
    StationManager* manager=new StationManager(pStationList, selectedIndexs);
    if (!selectedJustOne(selectedIndexs))
    {
        //选择了多个
        menu->addButton(":/Icons/52design.com_jingying_108.png", QStringLiteral("开机"), manager, SLOT(powerOn()));
        menu->addButton(":/Icons/200969173136504.png", QStringLiteral("重启"), manager, SLOT(reboot()));
        menu->addButton(":/Icons/52design.com_jingying_098.png", QStringLiteral("关机"), manager, SLOT(shutdown()));
        menu->addButton(":/Icons/52design.com_jingying_059.png", QStringLiteral("启动程序"), manager, SLOT(startApp()));
        menu->addButton(":/Icons/2009724113238761.png", QStringLiteral("重启程序"), manager,SLOT(restartApp()));
        menu->addButton(":/Icons/52design.com_alth_171.png", QStringLiteral("退出程序"), manager, SLOT(exitApp()));
    }
    else
    {
        //只选择了一个,则根据状态显示菜单
        auto station = pStationList->atCurrent(selectedIndexs.first().row());
        if (station->state() == StationInfo::Unkonown)
        {
            menu->addButton(":/Icons/52design.com_jingying_108.png", QStringLiteral("开机"), manager, SLOT(powerOn()));
            menu->addButton(":/Icons/200969173136504.png", QStringLiteral("重启"));
            menu->addButton(":/Icons/52design.com_jingying_098.png", QStringLiteral("关机"));
            menu->addButton(":/Icons/52design.com_jingying_059.png", QStringLiteral("启动程序"));
            menu->addButton(":/Icons/2009724113238761.png", QStringLiteral("重启程序"));
        }
        else if(station->state() == StationInfo::Powering)
        {
            menu->addButton(":/Icons/52design.com_jingying_108.png", QStringLiteral("开机"), manager, SLOT(powerOn()));
            menu->addButton(":/Icons/200969173136504.png", QStringLiteral("重启"));
            menu->addButton(":/Icons/52design.com_jingying_098.png", QStringLiteral("关机"));
        }
        else
        {
            menu->addButton(":/Icons/200969173136504.png", QStringLiteral("重启"), manager, SLOT(reboot()));
            menu->addButton(":/Icons/52design.com_jingying_098.png", QStringLiteral("关机"), manager, SLOT(shutdown()));
            if (station->AppIsRunning())
            {
                menu->addButton(":/Icons/2009724113238761.png", QStringLiteral("重启程序"), manager, SLOT(restartApp()));
                menu->addButton(":/Icons/52design.com_alth_171.png", QStringLiteral("退出程序"), manager, SLOT(exitApp()));
                menu->addButton(":/Icons/52design.com_3d_08.png", QStringLiteral("强制退出"), manager, SLOT(forceExitApp()));
            }
            else
            {
                menu->addButton(":/Icons/52design.com_jingying_059.png", QStringLiteral("启动程序"), manager, SLOT(startApp()));
            }
        }
    }
}

//获取当前选择的工作站索引
QModelIndexList MainWindow::getSelectedIndexs()
{
    QModelIndexList selectedIndexs;
    if (ui->listView->isVisible())
    {
        //当前是listView
        QItemSelectionModel* selection = ui->listView->selectionModel();
        selectedIndexs = selection->selectedIndexes();
    }
    else
    {
        //当前是 tableView
        QItemSelectionModel* selection = ui->tableView->selectionModel();
        selectedIndexs = selection->selectedIndexes();
    }

    return selectedIndexs;
}

//是否只选择了一个工作站
bool MainWindow::selectedJustOne(QModelIndexList selectedIndexs)
{
    if (ui->listView->isVisible())
        return selectedIndexs.count() == 1;
    //如果是表格,则做进一步判断   
    if (selectedIndexs.isEmpty())
        return false;
    int row = selectedIndexs.first().row();
    for (auto index : selectedIndexs)
    {
        if (row != index.row())
            return false;
    }
    return true;
}
