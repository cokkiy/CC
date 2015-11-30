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
    
private:
    Ui::MainWindow *ui;

    // 工作站信息加载完毕
    void configLoaded(StationList* pStations);

    //小图标模式（默认模式)
    void smallIcomMode();

    //工作站信息加载失败
    void configLoadFailed(const QString& fileName);

    //加载配置文件线程
    QThread ldconf_thread;
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

    // Create an ice communicator 
    // Ice communicator for receiving station state
    Ice::CommunicatorPtr communicator;
    // Adapter for communicator 
    Ice::ObjectAdapterPtr adapter;
    //ICE框架初始化是否成功
    bool iceInitSuccess = false;

private slots:
    //点击标题栏排序
    void sortByColumn(int cloumnIndex);
    //显示悬浮式菜单
    void showFloatingMenu(const QPoint & pos);
    // 操作菜单即将显示
    void operationMenuToShow();
    //工作站状态发生变化
    void stationStateChanged(const QObject*);
    //清空消息记录
    void clearMessage();
    //拷贝选择的消息
    void copyMessage();
    //拷贝全部消息
    void copyAllMessage();
private:
    //添加按钮到右键菜单
    void addButtons(FloatingMenu* menu);
    //获取当前选择的工作站索引
    QModelIndexList getSelectedIndexs();    
    //是否只选择了一个工作站
    bool selectedJustOne(QModelIndexList selectedIndexs);

};

//Class for config loading thread
class ConfigLoader : public QObject
{
    Q_OBJECT
public:
    ConfigLoader() {};
    ~ConfigLoader() {};
    public slots:
    //加载工作站信息
    void load(const QString filename, StationList* pStations)
    {
        XDocument doc;
        if (doc.Load(filename))
        {
            auto elements = doc.getChild().getChildrenByName(QString::fromLocal8Bit("指显工作站"));
            for (XElement el : elements)
            {
                StationInfo station;
                station.name = el.getChildValue(QString::fromLocal8Bit("名称"));
                station.IP = el.getChildValue(QString::fromLocal8Bit("IP"));
                station.mac = el.getChildValue(QString::fromLocal8Bit("MAC"));
                pStations->push(station);
            }
            emit loaded(pStations);
        }
        else
        {
            emit fail(filename);
        }
    }
signals:
    // the config file is loaded
    void loaded(StationList* pStations);
    // load file error
    void fail(const QString filename);
};

#endif // MAINWINDOW_H
