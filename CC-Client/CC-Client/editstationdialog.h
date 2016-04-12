#pragma once
#include <QDialog>
#include "StationInfo.h"
#include <QValidator>
#include <QRegExp>

namespace Ui {class EditStationDialog;}

/*
 编辑工作站信息对话框
 **/
class EditStationDialog : public QDialog
{
    Q_OBJECT

public:
    /*
     编辑类型
     **/
    enum EditType
    {
        /*
         编辑
         **/
        Edit,
        /*
        新建
        **/
        New,
    };
    EditStationDialog(StationInfo* station, EditType editType = EditType::Edit, QWidget *parent = 0);
    ~EditStationDialog();

    public slots:
    //增加网卡配置按钮
    void on_addNiPushButton_clicked();   
    //删除网卡配置
    void on_removeNIPushButton_clicked();
    //确定
    void on_OKPushButton_clicked();
    //取消
    void on_CancelPushButton_clicked();
    //添加启动程序
    void on_addStartAppPushButton_clicked();
    //删除启动程序
    void on_removeStartAppPushButton_clicked();
    //添加监视进程
    void on_addMonitorProcPushButton_clicked();
    //删除监视进程
    void on_removeMonitorProcPushButton_clicked();
    //查看当前运行进程
    void on_viewProcPushButton_clicked();
private:
    //初始化UI
    void initUI();
    //添加网卡配置控件
    void AddNIConfigControl(QString mac,QString IPs);
    //更新工作站数据
    void UpdateStation();
private:
    Ui::EditStationDialog *ui;
    StationInfo* station;
    QRegExp macRegExp, ipRegExp;
    QRegExpValidator macValidator, ipValidator;
};

