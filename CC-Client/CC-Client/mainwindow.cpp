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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //创建更新定时器
    updateTimer = new QTimer();
    //connect(updateTimer, SIGNAL(timeout()), this, SLOT(update()));
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
    delete updateTimer;
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
    ui->tableView->setModel(pTableModel);
    ui->listView->setModel(pTableModel);
    //绑定鼠标进入事件,显示悬浮式菜单
    connect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showFloatingMenu(const QPoint &)));
    connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showFloatingMenu(const QPoint &)));
    //默认小图标模式
    smallIcomMode();
    //启动更新定时器
    //updateTimer->start(1000);
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
void MainWindow::showFloatingMenu(const QModelIndex & index)
{
    //悬浮式菜单    
//     FloatingMenu* menu = new FloatingMenu(this);
//     //menu->setGeometry(this->cursor().pos().x(), this->cursor().pos().y(), 180, 40);
//     menu->show(this->cursor().pos().x(), this->cursor().pos().y());
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
    if (buildDlg.exec() == QDialog::Accepted)
    {
        FilterOperations operations = buildDlg.getFilter();
        ui->filterLineEdit->setText(operations.toString());
    }
}

//添加按钮到右键菜单
void MainWindow::addButtons(FloatingMenu* menu)
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
    //创建工作站管理类
    StationManager* manager=new StationManager(pStationList, selectedIndexs);
    if (selectedIndexs.count() > 1)
    {
        menu->addButton(":/Icons/52design.com_jingying_108.png", QStringLiteral("开机"), manager, SLOT(powerOn()));
        menu->addButton(":/Icons/200969173136504.png", QStringLiteral("重启"), manager, SLOT(reboot()));
        menu->addButton(":/Icons/52design.com_jingying_098.png", QStringLiteral("关机"), manager, SLOT(shutdown()));
        menu->addButton(":/Icons/52design.com_jingying_059.png", QStringLiteral("启动指显"), manager, SLOT(startApp()));
        menu->addButton(":/Icons/2009724113238761.png", QStringLiteral("重启指显"), manager,SLOT(restartApp()));
        menu->addButton(":/Icons/52design.com_alth_171.png", QStringLiteral("退出指显"), manager, SLOT(exitApp()));
    }
    else
    {
        //根据状态选择
        auto station = pStationList->at(selectedIndexs.first().row());
        if (station->state() == StationInfo::Unkonown)
        {
            menu->addButton(":/Icons/52design.com_jingying_108.png", QStringLiteral("开机"), manager, SLOT(powerOn()));
            menu->addButton(":/Icons/200969173136504.png", QStringLiteral("重启"));
            menu->addButton(":/Icons/52design.com_jingying_098.png", QStringLiteral("关机"));
            menu->addButton(":/Icons/52design.com_jingying_059.png", QStringLiteral("启动指显"));
            menu->addButton(":/Icons/2009724113238761.png", QStringLiteral("重启指显"));
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
            if (station->ZXIsRunning())
            {
                menu->addButton(":/Icons/2009724113238761.png", QStringLiteral("重启指显"), manager, SLOT(restartApp()));
                menu->addButton(":/Icons/52design.com_alth_171.png", QStringLiteral("退出指显"), manager, SLOT(exitApp()));
                menu->addButton(":/Icons/52design.com_3d_08.png", QStringLiteral("强制退出"), manager, SLOT(forceExitApp()));
            }
            else
            {
                menu->addButton(":/Icons/52design.com_jingying_059.png", QStringLiteral("启动指显"), manager, SLOT(startApp()));
            }
        }
    }
}
