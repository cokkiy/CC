#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
/**************************
\class Option 选项
 **************************/
class Option : public QObject
{
    Q_OBJECT

public:
    Option(QObject *parent=NULL);
    ~Option();
public:
    /*
     需要启动的程序列表
     **/
    std::list<std::pair<QString,QString>> StartApps;
    /*
    需要监视的进程列表
    **/
    std::list<QString> MonitorProcesses;

    /*
     监视间隔
     **/
    int Interval;

    /*
    自动监视远程启动程序
    */
    bool AutoMonitorRemoteStartApp;

    /*
     *是否是第一次运行,如果是,则启动就显示默认程序配置对话框
     **/
    bool IsFirstTimeRun;

    /*!
    保存配置
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/04 12:42:27
    */
    void Save();

    /*!
    加载配置
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/04 12:42:50
    */
    void Load();

    /*!
    获取用户选项实例
    @return Option* 用户选项实例
    作者：cokkiy（张立民)
    创建时间：2015/12/04 16:34:23
    */
    static Option* getInstance()
    {
        if (Instance == nullptr)
        {
            Instance = new Option;
            Instance->Load();
        }
        return Instance;
    }
private:
    /*用户选项的实例*/
    static Option* Instance;
};

#endif // CONFIG_H
