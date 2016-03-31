// TestCConsole.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
namespace CC
{
    enum RunningState
    {
        Unknown = 0,/*未知*/
        Normal = 1,/*正常*/
        Warning,/*告警*/
        Error,/*异常*/
    };
    /*
    工作站操作状态
    **/
    enum OperatingStatus
    {
        PowerOffOrNetworkFailure,/*没有加电或网络故障*/
        Powering, /*加电中...*/
        AppStarting,/*软件启动中*/
        Shutdowning,/*关机中...*/
        Rebooting,/*重启中*/
        PowerOnFailure,/*加电失败*/
        AppStartFailure,/*指显程序启动失败*/
        ShutdownFailure,/*关机失败*/
        RebootFailure,/*重启失败*/
        MemoryTooHigh,/*内存占用率太高*/
        CPUTooHigh,/*CPU占用率太高*/
        DiskFull,/*磁盘满*/
    };

    class State 
    {
    public:
        /*默认构造函数*/
        State() = default;
        /*!
        设置运行状态
        @param RunningState runningState
        @return
        作者：cokkiy（张立民)
        创建时间：2015/12/03 21:33:25
        */
        State(const RunningState& runningState)
        {
            this->runningState = runningState;
        }

        /*
        设置操作状态(操作状态会改变运行状态)
        **/
        State(const OperatingStatus& opStatus)
        {
            this->operatingStatus = opStatus;
        }
    public:
        //     /*判断运行状态是否相等*/
        //     bool operator ==(const RunningState& state);
        //     /*判断操作状态是否相等*/
        //     bool operator ==(const OperatingStatus& status);
        //     /*判断运行状态是否不等于*/
        //     bool operator!=(const RunningState& state);
        //     /*判断操作状态是否不等于*/
        //     bool operator !=(const OperatingStatus& status);
        //     /*两个状态是否不相等*/
        //     bool operator != (const State& right);
        //     /*两个状态是否相等*/
        //     bool operator ==(const State& right);

            /*!
            设置操作状态和附加消息
            @param const OperatingStatus & status 操作状态
            @param QString attachMessage 附加消息
            @return void
            作者：cokkiy（张立民)
            创建时间：2015/12/04 9:10:31
            */
            // void setStatus(const OperatingStatus& status);

             /*!
             获取当前运行状态
             @return StationInfo::State::RunningState
             作者：cokkiy（张立民)
             创建时间：2015/12/04 9:24:57
             */
             //RunningState getRunningState();

             /*!
             获取当前操作状态
             @return StationInfo::State::OperatingStatus
             作者：cokkiy（张立民)
             创建时间：2015/12/04 9:24:41
             */
             //OperatingStatus getOperatingStatus();
    private:
        /*
        工作站运行状态
        **/
        RunningState runningState;
        /*
        工作站操作状态
        **/
        OperatingStatus operatingStatus;
        /*
        附加的消息
        **/

        /*
        设置操作状态
        **/
        //void setOperatingStatus(const   OperatingStatus& opStatus);
    protected:
        ~State() {}

        friend class StateInit;
    };
    class StateInit
    {
    public:
        State stateInit;
    };


    static StateInit _stateInit;
}
int main()
{
    //CC::State s = CC::Warning;
    //s = CC::AppStartFailure;
    return 0;
}

