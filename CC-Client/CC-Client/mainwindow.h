#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thread>
#include <list>
#include "StationInfo.h"
#include "xdocument.h"
#include <QThread>
#include <QHeaderView>
#include "StationList.h"
#include "stationtablemodel.h"
#include "floatingmenu.h"
#include <Ice\Ice.h>
#include "StationStateReceiver.h"
#include "monitor.h"
#include "option.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "cpuplot\plotpart.h"

//性能计数器最多个数
#define  MaxCountOfCounter 20

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /*!
    重载show函数,实现加载配置信息
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/03 15:43:42
    */
    virtual void show();

    /*!
    关闭程序前提示确认
    @return bool
    作者：cokkiy（张立民)
    创建时间：2015/11/04 15:43:17
    */
    virtual void closeEvent(QCloseEvent * event);

    private slots:
    //选择机子开机
    void on_actionPower_On_triggered();
    //全部开机
    void on_actionAll_Power_On_triggered();
    //排序方式发生变化
    void on_sortComboBox_currentIndexChanged(int index);
    //显示方式发生变化
    void on_displayModeComboBox_currentIndexChanged(QString index);
    //点击过滤按钮
    void on_filterToolButton_clicked();
    //查看CPU占用率
    void on_actionViewCPU_triggered(bool checked);
    //查看内存占用率
    void on_actionViewMemory_triggered(bool checked);
    //查看详细信息
    void on_actionViewDetail_triggered(bool checked);
    //查看消息
    void on_actionViewMessage_triggered(bool checked);
    //消息列表右键菜单
    void on_msgListWidget_customContextMenuRequested(const QPoint &);
    //设置监视间隔
    void on_actionSetInterval_triggered();
    //自动更新工作站列表
    void on_actionAllowAutoRefreshList_triggered(bool checked);
    //新加工作站
    void on_actionNewStation_triggered();
    //编辑工作站
    void on_actionEdit_triggered();
    //删除工作站
    void on_actionRemove_triggered();
    //设置新建工作站默认监视进程和启动程序
    void on_actionSetDefaultAppAndProc_triggered();
    //查看工作站详细信息
    void on_actionViewStationDetail_triggered();

private:
    Ui::MainWindow *ui;

    // 工作站信息加载完毕
    void stationsLoaded(StationList* pStations, bool success);

    //小图标模式（默认模式)
    void smallIcomMode();

    //加载配置文件线程
    QThread ldstation_thread;
signals:
    //加载配置文件
    void load(const QString& fileName, StationList* pStations);

private:
    //工作站列表
    StationList* pStationList=nullptr;
    //工作站表模型
    StationTableModel *pTableModel = nullptr;
    //table header
    QHeaderView* tableHeader;

    //监视线程
    Monitor* monitor = nullptr;

    //工作站文件目录
    QString filePath;
    //工作站文件名称
    QString fileName;

    // Create an ice communicator 
    // Ice communicator for receiving station state
    Ice::CommunicatorPtr communicator;
    // Adapter for communicator 
    Ice::ObjectAdapterPtr adapter;
    //ICE框架初始化是否成功
    bool iceInitSuccess = false;
    //工作站信息接收指针
    StationStateReceiver* StationStateReceiverPtr = nullptr;
    //用户选项
    Option option;

    //性能曲线数组
    PerfCurves perfCurves[MaxCountOfCounter];

    //当前选择的工作站
    StationInfo* currentStation = nullptr;
    //CPU,内存记录时间数据
    double timeData[CounterHistoryDataSize];
private slots:
    //点击标题栏排序
    void sortByColumn(int cloumnIndex);
    //显示悬浮式菜单
    void showFloatingMenu(const QPoint & pos);
    // 操作菜单即将显示
    void operationMenuToShow();
    //工作站状态发生变化
    void stationStateChanged(const QObject*);
    //工作站被添加事件处理函数
    void onStationAdded(StationInfo* addedStation);
    //清空消息记录
    void clearMessage();
    //拷贝选择的消息
    void copyMessage();
    //拷贝全部消息
    void copyAllMessage();
    //工作站列表发生变化
    void onStationListChanged();
    //工作站发生变化
    void stationChanged(const QObject*);
    //当前选择的工作站发生变化
    void currentStationChanged(const QModelIndex &);
private:
    //添加按钮到右键菜单
    void addButtons(FloatingMenu* menu);
    //获取当前选择的工作站索引
    QModelIndexList getSelectedIndexs();    
    //是否只选择了一个工作站
    bool selectedJustOne(QModelIndexList selectedIndexs);
    //设置QwtPlot样式
    void setPlotStyle(QwtPlot* plot, QString title);
protected:
        void timerEvent(QTimerEvent *e);
};

//Class for station loading thread
class StationsLoader : public QObject
{
    Q_OBJECT
public:
    StationsLoader() {};
    ~StationsLoader() {};
    public slots:
    //加载工作站信息
    void load(const QString filename, StationList* pStations);
signals:
    /*!
    列表加载完毕信号,当工作站加载完毕时被触发
    @param StationList * pStations 工作站列表
    @param bool success 是否成功加载
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/03 9:20:23
    */
    void loaded(StationList* pStations,bool success);
};

#endif // MAINWINDOW_H


