#include "StationInfo.h"
#include <QObject>
#include <assert.h>
using namespace std;

bool StationInfo::IsRunning()
{
    return m_state != Unknown;
}


bool StationInfo::NeedUpdate()
{
	return m_NeedUpdate;
}

void StationInfo::setNeedUpdate(bool value)
{
	if (m_NeedUpdate != value)
	{
		m_NeedUpdate = value;		
		if (m_NeedUpdate)
			m_state.setAttachMessage(QStringLiteral("需要升级"));
		emit propertyChanged("NeedUpdate", this);
		emit stateChanged(this); //通知激发状态变化通知
	}
}

QString StationInfo::Name()
{
    return m_Name;
}

void StationInfo::setName(QString value)
{
    if (m_Name != value)
    {
        m_Name = value;
        emit propertyChanged("Name", this);
    }
}

int StationInfo::ExecuteCounting()
{
    return m_ExecuteCounting;
}

void StationInfo::setExecuteCounting(int value)
{
    if (m_ExecuteCounting != value)
    {
        m_ExecuteCounting = value;
        m_state.setAttachMessage(QStringLiteral("命令已发送%1秒...").arg(m_ExecuteCounting));
        emit propertyChanged("ExecuteCounting", this);
    }
}


float StationInfo::AppMemory()
{
    return m_ZXMemory;
}

void StationInfo::setAppMemory(float value)
{
    if (m_ZXMemory != value)
    {
        m_ZXMemory = value;
        emit propertyChanged("AppMemory", this);
    }
}

size_t StationInfo::TotalMemory()
{
    return m_TotalMemory;
}

void StationInfo::setTotalMemory(size_t value)
{
    if (m_TotalMemory != value)
    {
        m_TotalMemory = value;
        emit propertyChanged("TotalMemory", this);
    }
}

int StationInfo::AppThreadCount()
{
    return m_zxThreadCount;
}

void StationInfo::setAppThreadCount(int value)
{
    if (m_zxThreadCount != value)
    {
        m_zxThreadCount = value;
        emit propertyChanged("AppThreadCount", this);
    }
}

int StationInfo::ProcCount()
{
    return m_ProcCount;
}

void StationInfo::setProcCount(int value)
{
    if (m_ProcCount != value)
    {
        m_ProcCount = value;
        emit propertyChanged("ProcCount", this);
    }
}

float StationInfo::AppCPU()
{
    return m_zxCPU;
}

void StationInfo::setAppCPU(float value)
{
    if (m_zxCPU != value)
    {
        m_zxCPU = value;
        emit propertyChanged("AppCPU", this);
    }
}

float StationInfo::Memory()
{
    return m_Memory;
}

void StationInfo::setMemory(float value)
{
    for (int i = CounterHistoryDataSize - 1; i > 0; i--)
    {
        counterData["Total"].memoryData[i] = counterData["Total"].memoryData[i - 1];
        counterData["Idle"].memoryData[i] = counterData["Idle"].memoryData[i - 1];
    }
    counterData["Total"].memoryData[0] = value / TotalMemory() * 100;
    counterData["Idle"].memoryData[0] = 100.0 - counterData["Total"].memoryData[0];
    if (m_Memory != value)
    {
        m_Memory = value;
        emit propertyChanged("Memory", this);
    }
}

StationInfo::State StationInfo::state()
{
    return m_state;
}

//设置操作状态和消息
void StationInfo::setState(OperatingStatus status, const QString& message)
{
	if (m_state != status)
	{
		if (message == QStringLiteral("") && m_NeedUpdate)
		{
			m_state.setStatus(status, QStringLiteral("需要升级"));
		}
		else
		{
			m_state.setStatus(status, message);
		}
		
		if (m_state == Error)
		{
			lastTick = time(NULL);
		}
		operatingHistoryMessages.push_back(m_state.toHtmlString());
		emit propertyChanged("state", this);
		emit stateChanged(this);
	}
}

float StationInfo::CPU()
{
    return m_CPU;
}

void StationInfo::setCPU(float value)
{
    for (int i = CounterHistoryDataSize - 1; i > 0; i--)
    {
        counterData["Total"].cpuData[i] = counterData["Total"].cpuData[i - 1];
        counterData["Idle"].cpuData[i] = counterData["Idle"].cpuData[i - 1];
    }
    counterData["Total"].cpuData[0] = value;
    counterData["Idle"].cpuData[0] = 100.0 - value;
    if (m_CPU != value)
    {
        m_CPU = value;
        emit propertyChanged("CPU", this);
    }
}

/*复制构造函数*/
StationInfo::StationInfo(const StationInfo& ref)
{
    this->m_Name = ref.m_Name;
    this->NetworkIntefaces = ref.NetworkIntefaces;
    this->m_state = ref.m_state;
    this->m_CPU = ref.m_CPU;
    this->m_Memory = ref.m_Memory;
    this->m_ProcCount = ref.m_ProcCount;
    this->m_TotalMemory = ref.m_TotalMemory;
    this->m_zxThreadCount = ref.m_zxThreadCount;
    this->m_zxCPU = ref.m_zxCPU;
    this->m_ZXMemory = ref.m_ZXMemory;
    this->m_ExecuteCounting = ref.m_ExecuteCounting;
    this->stationId = ref.stationId;
    this->lastTick = ref.lastTick;
    this->startAppList = ref.startAppList;
    this->standAloneMonitorProcessList = ref.standAloneMonitorProcessList;
}


/*!
是否在执行状态
@return bool 如果在执行状态,返回true,否则返回false
@remarks 执行状态包括：AppStarting Powering Shutdowning Rebooting
作者：cokkiy（张立民)
创建时间：2015/11/12 21:27:35
*/
bool StationInfo::inExecutingState()
{
    return state() == AppStarting || state() == Powering
        || state() == Shutdowning || state() == Rebooting;
}

/*!
设置工作站标识
@param const std::string & stationId 工作站唯一标识
@return void
作者：cokkiy（张立民)
创建时间：2015/11/24 10:41:20
*/
void StationInfo::setStationId(const std::string& stationId)
{
    this->stationId = QString(stationId.c_str());
}

/*!
设置工作站操作系统名称
@param const std::string & osName 工作站操作系统名称
@return void
作者：cokkiy（张立民)
创建时间：2015/11/24 10:41:20
*/
void StationInfo::setOSName(const std::string& osName)
{
    this->osName = QString(osName.c_str());
    emit propertyChanged("osName", this);
}

/*!
设置工作站操作系统版本
@param const std::string & osVersion 工作站操作系统版本
@return void
作者：cokkiy（张立民)
创建时间：2015/11/24 10:41:20
*/
void StationInfo::setOSVersion(const std::string& osVersion)
{
    this->osVersion = QString(osVersion.c_str());
    emit propertyChanged("osVersion", this);
}

/*!
设置最后收到状态时间并计算状态
@param const time_t & time 收到状态的时间
@return void
作者：cokkiy（张立民)
创建时间：2015/11/25 12:17:13
*/
void StationInfo::setLastTick(const time_t& tickTime /*= time(NULL)*/)
{
	this->lastTick = tickTime;
	if (m_state == Error && tickTime - lastErrorTime < 5)
	{
		//错误状态需要保持5秒
		return;
	}
	//不改变执行状态
	if (inExecutingState() && m_state != Powering)
		return;

	//检查CPU占用率,内存占用率和磁盘占用率
	if (m_CPU > 90)
	{
		setState(CPUTooHigh);
		return;
	}

	if (m_Memory / m_TotalMemory > 0.9)
	{
		setState(MemoryTooHigh);
		return;
	}

	//检查程序运行状态
	if (AppsRunningState.size() >= getShouldMonitoredProcessesCount())
	{
		//结果和监视数量一致
		auto running=find_if(AppsRunningState.begin(), AppsRunningState.end(), 
			[](CC::AppRunningState& state) { return state.isRunning;});
		auto notRunning = find_if(AppsRunningState.begin(), AppsRunningState.end(),
			[](CC::AppRunningState& state) { return !state.isRunning;});
		if (notRunning == AppsRunningState.end())
		{
			//全部都在运行
			setState(AppStarted);
		}
		else if(running== AppsRunningState.end())
		{
			//全部没有运行
			setState(AppNotRunning);
		}
		else
		{
			//部分运行
			setState(SomeAppNotRunning, QStringLiteral("详情可通过\"详细信息\"查看."));
		}
	}
	else
	{
		//结果和监视数量不一致
		auto running = find_if(AppsRunningState.begin(), AppsRunningState.end(),
			[](CC::AppRunningState& state) { return state.isRunning;});
		if (running == AppsRunningState.end())
		{
			//全部没有运行
			setState(AppNotRunning);
		}
		else
		{
			//至少有一个在运行
			setState(SomeAppNotRunning, QStringLiteral("详情可通过\"详细信息\"查看."));
		}
	}
	
}

/*!
返回描述工作站的字符串
@return QString 描述工作站的字符串
作者：cokkiy（张立民)
创建时间：2015/12/01 16:35:23
*/
QString StationInfo::toString()
{
    QString allIP = QStringLiteral("");
    for (auto& ni : NetworkIntefaces)
    {
        allIP += ni.getAllIPString() + ",";
    }
    allIP = allIP.left(allIP.length() - 1);
    return QStringLiteral("%1 IP(%2)").arg(m_Name).arg(allIP);
}

/*!
获取表示所有IP地址的字符串,多个IP用逗号分隔
@return QString 表示所有IP地址的字符串,多个IP用逗号分隔
作者：cokkiy（张立民)
创建时间：2015/12/01 16:50:44
*/
QString StationInfo::IP()
{
    QString allIP = QStringLiteral("");
    for (auto& ni : NetworkIntefaces)
    {
        allIP = allIP.append(ni.getAllIPString()).append(",");
    }
    return allIP.left(allIP.length() - 1);
}

/*!
获取表示所有MAC地址的字符串,多个MAC地址用逗号分隔
@return QString 表示所有MAC地址的字符串,多个MAC地址用逗号分隔
作者：cokkiy（张立民)
创建时间：2015/12/01 17:09:46
*/
QString StationInfo::MAC()
{
    QString result;
    for (auto& ni : NetworkIntefaces)
    {
        result = result.append(ni.getMAC()).append(",");
    }
    return result.left(result.length() - 1);
}

/*!
返回工作站所有配置信息的XML字符串
@return QString 工作站所有配置信息的XML字符串
作者：cokkiy（张立民)
创建时间：2015/12/02 22:49:13
*/
QString StationInfo::toXmlString()
{
    QString xml = QStringLiteral("  <指显工作站>\r\n")
        + QStringLiteral("    <名称>%1</名称>\r\n").arg(m_Name);
    for (auto&ni : NetworkIntefaces)
    {
        xml += QStringLiteral("    <网卡>\r\n")
            + QStringLiteral("      <MAC>%1</MAC>\r\n").arg(ni.getMAC());
        for (auto&ip : ni.getIPList())
        {
            xml += QStringLiteral("      <IP>%1</IP>\r\n").arg(QString::fromStdString(ip));
        }
        xml += QStringLiteral("    </网卡>\r\n");
    }
    xml += QStringLiteral("    <启动程序>\r\n");
    for (auto& app : getStartAppNames())
    {
        xml += QStringLiteral("      <程序>\r\n"
            "        <路径>%1</路径>\r\n"
            "        <参数>%2</参数>\r\n"
            "        <进程名>%3</进程名>\r\n"
            "        <允许多实例>%4</允许多实例>\r\n"
            "     </程序>\r\n").arg(QString::fromStdString(app.AppPath))
            .arg(QString::fromStdString(app.Arguments))
            .arg(QString::fromStdString(app.ProcessName))
            .arg(app.AllowMultiInstance);
    }
    xml += QStringLiteral("    </启动程序>\r\n");
    xml += QStringLiteral("    <监视进程>\r\n");
    for (auto& proc : getStandAloneMonitorProcNames())
    {
        xml += QStringLiteral("      <进程>%1</进程>\r\n").arg(proc);
    }
    xml += QStringLiteral("    </监视进程>\r\n");
    xml += QStringLiteral("  </指显工作站>\r\n");
    return xml;
}



void StationInfo::clearStandAloneMonitorProc()
{
    this->standAloneMonitorProcessList.clear();
}

void StationInfo::addStandAloneMonitorProcess(const QString& procName)
{
    this->standAloneMonitorProcessList.push_back(procName);
}

void StationInfo::addStandAloneMonitorProcesses(const std::list<QString>& processes)
{
    this->standAloneMonitorProcessList.append(QStringList::fromStdList(processes));
}

void StationInfo::clearOperatingHistoryMessages()
{
    operatingHistoryMessages.clear();
}

std::list<QString> StationInfo::getOperatingHistoryMessages()
{
    return operatingHistoryMessages;
}

/*!
获取需要启动程序列表
@returnstd::list<CC::AppStartParameter> 需要启动程序名称,参数和对应进程名列表
作者：cokkiy（张立民)
创建时间：2015/12/01 22:25:20
*/
std::list<CC::AppStartParameter> StationInfo::getStartAppNames()
{
    return startAppList;
}

void StationInfo::addStartApp(const QString& path, const QString& arguments, const QString& procName, bool allowMultiInstance)
{
    CC::AppStartParameter param{ (int)startAppList.size(), path.toStdString(), arguments.toStdString(), procName.toStdString(), allowMultiInstance };
    startAppList.push_back(param);
}

void StationInfo::addStartApps(const std::list<CC::AppStartParameter>& apps)
{
    startAppList.insert(startAppList.end(), apps.begin(), apps.end());
}

void StationInfo::clearStartApp()
{
    startAppList.clear();
}

QStringList StationInfo::getStandAloneMonitorProcNames()
{
    return standAloneMonitorProcessList;
}

void StationInfo::UpdateStation()
{
    emit stationChanged(this);
}

/*!
检查是否执行动作是否超时
@return bool
作者：cokkiy（张立民)
创建时间：2015/12/10 20:51:14
*/
bool StationInfo::timeout()
{
    switch (this->state().getOperatingStatus())
    {
    case StationInfo::Powering:
        return this->ExecuteCounting() > PowerOnTimeout;
        break;
    case  StationInfo::AppStarting:
        return this->ExecuteCounting() > AppStartingTimeout;
        break;
    case  StationInfo::Rebooting:
        return this->ExecuteCounting() > RebootingTimeout;
        break;
    case  StationInfo::Shutdowning:
        return this->ExecuteCounting() > ShutingdownTimeout;
        break;
    default:
        return false;
        break;
    }
}

/*!
获取所有应该监视的进程名称列表
@return std::list<std::string> 所有应该监视的进程名称列表
作者：cokkiy（张立民)
创建时间：2015/12/11 9:01:20
*/
std::list<std::string> StationInfo::getAllShouldMonitoredProcessesName()
{
    list<string> processes;
    for (auto& process : standAloneMonitorProcessList)
    {
        processes.push_back(process.toStdString());
    }   
    for (auto&param : startAppList)
    {
        processes.push_back(param.ProcessName);
    }
    return processes;
}


/*!
根据参数标识获取程序名称
@param int ParamId 参数标识
@return QString 参数标识对应的程序名称
作者：cokkiy（张立民)
创建时间：2015/12/24 9:19:23
*/
QString StationInfo::getStartAppNameByIndex(int ParamId)
{
    for (auto& param : startAppList)
    {
        if (param.ParamId == ParamId)
            return QString::fromStdString(param.AppPath);
    }

    return QStringLiteral("");
}


/*!
获取所有已运行(远程启动)程序进程ID列表
@return list<int>
作者：cokkiy（张立民）
创建时间:2016/3/9 9:09:20
*/
list<int> StationInfo::getStartedAppProcessIds()
{
	list<int> allIds;
	for (auto& runningState : AppsRunningState)
	{
		if (runningState.isRunning)
		{
			auto app = find_if(startAppList.begin(), startAppList.end(),
				[&](const CC::AppStartParameter param) { return param.ProcessName == runningState.Process.ProcessMonitorName;});
			if (app != startAppList.end())
			{
				allIds.push_back(runningState.Process.Id);
			}
		}
	}
	return allIds;
}

/*!
判断指定名称和修改时间的文件是否已经发送
@param QString fileName 文件名
@param time_t lastModify 最后修改时间
@return bool
作者：cokkiy（张立民）
创建时间:2017/5/3 11:18:03
*/
bool StationInfo::isSended(const QString& fileName, const QDateTime& lastModify)
{
	return std::any_of(sendedFiles.begin(), sendedFiles.end(),
		[&](std::pair<QString, QDateTime> sendedFile) {
		return sendedFile.first == fileName && sendedFile.second == lastModify;});
}

/*!
添加指定文件名和最后修改时间到已发送文件列表
@param QString fileName 文件名
@param time_t lastModify 最后修改时间
@return void
作者：cokkiy（张立民）
创建时间:2017/5/3 11:19:18
*/
void StationInfo::addToSendedFileList(const QString& fileName, const QDateTime& lastModify)
{
	sendedFiles[fileName] = lastModify;
}

void StationInfo::addSendFileLog(const QString& log, bool isError /*= false*/)
{
	if (!log.isEmpty() && !log.isNull())
	{
		sendFileLogs.push_back(log);
		if(isError)
			hasErrorWhenSendFile = isError;
	}
}

bool StationInfo::getHasErrorWhenSendFile()
{
	return hasErrorWhenSendFile;
}

void StationInfo::clearSendFileLog()
{
	hasErrorWhenSendFile = false;
	sendFileLogs.clear();
}

std::list<QString> StationInfo::getSendFileLog()
{
	return sendFileLogs;
}

/*!
获取工作站监视进程运行状态
@return ::CC::AppsRunningState
作者：cokkiy（张立民）
创建时间:2016/3/31 16:23:05
*/
::CC::AppsRunningState StationInfo::getAppRunningState()
{
	return this->AppsRunningState;
}

/*!
获取应该监视进程数量
@return int 应该监视进程数量=监视进程列表数量+远程启动程序数量
作者：cokkiy（张立民)
创建时间：2015/12/14 15:36:46
*/
int StationInfo::getShouldMonitoredProcessesCount()
{
    return standAloneMonitorProcessList.size() + startAppList.size();
}


/*!
设置应用程序运行状态
@param const ::CC::AppsRunningState & appsRunningState 应用程序运行状态
@return void
作者：cokkiy（张立民)
创建时间：2015/12/11 16:58:29
*/
void StationInfo::setAppsRunningState(const ::CC::AppsRunningState& appsRunningState)
{
    this->AppsRunningState.clear();
    this->AppsRunningState.insert(this->AppsRunningState.begin(), appsRunningState.begin(), appsRunningState.end());
    for (auto& runningState : this->AppsRunningState)
    {
        if (runningState.isRunning)
        {
            if (counterData.find(runningState.Process.ProcessName) != counterData.end())
            {
                //移动数据
                for (int i = CounterHistoryDataSize - 1; i > 0; i--)
                {
                    counterData[runningState.Process.ProcessName].cpuData[i] = counterData[runningState.Process.ProcessName].cpuData[i - 1];
                    counterData[runningState.Process.ProcessName].memoryData[i] = counterData[runningState.Process.ProcessName].memoryData[i - 1];
                }
            }
            counterData[runningState.Process.ProcessName].cpuData[0] = runningState.cpu;
			counterData[runningState.Process.ProcessName].memoryData[0] = runningState.currentMemory / (double)TotalMemory() * 100;
        }
    }
}

/*!
判断两个工作站信息是否是同一个对象
@param const StationInfo & ref 比较对象
@return bool 相同返回true,否则返回false
作者：cokkiy（张立民)
创建时间：2015/12/03 16:02:23
*/
bool StationInfo::operator==(const StationInfo& ref)
{
    return this == &ref;
}

/*!
订阅属性变化事件
@param const QObject * receiver 接收者
@param const char * member 接收者处理函数
@return void
作者：cokkiy（张立民)
创建时间：2015/11/12 9:35:22
*/
void StationInfo::subscribePropertyChanged(const QObject* receiver, const char* member)
{
    connect(this, SIGNAL(propertyChanged(const QString&, const QObject*)), receiver, member);
}

/*!
订阅状态发生变化事件
@param const QObject * receiver 接收者
@param const char * member 接收者处理函数
@return void
作者：cokkiy（张立民)
创建时间：2015/11/13 10:55:42
*/
void StationInfo::subscribeStateChanged(const QObject* receiver, const char* member)
{
    connect(this, SIGNAL(stateChanged(const QObject*)), receiver, member);
}

/*!
订阅由于用户编辑导致的工作站发生变化事件
@param const QObject * receiver 接收者
@param const char * member 接收者处理函数
@return void
作者：cokkiy（张立民)
创建时间：2015/11/13 10:55:42
*/
void StationInfo::subscribeStationChanged(const QObject* receiver, const char* member)
{
    connect(this, SIGNAL(stationChanged(const QObject*)), receiver, member);
}

bool StationInfo::State::operator!=(const OperatingStatus& status)
{
    return operatingStatus != status;
}

bool StationInfo::State::operator!=(const RunningState& state)
{
    return runningState != state;
}

bool StationInfo::State::operator==(const OperatingStatus& status)
{
    return operatingStatus == status;
}

bool StationInfo::State::operator==(const RunningState& state)
{
    return runningState == state;
}

StationInfo::State::State()
{
    runningState = Unknown;
    operatingStatus = PowerOffOrNetworkFailure;
}

/*
 两个状态是否不相同
 **/
bool StationInfo::State::operator!=(const State& right)
{
    return this->runningState != right.runningState
        || this->operatingStatus != right.operatingStatus;
}
/*
两个状态是否相同
**/
bool StationInfo::State::operator==(const State& right)
{
    return this->runningState == right.runningState
        && this->operatingStatus == right.operatingStatus;
}

/*是否大于等于right状态*/
bool StationInfo::State::operator>=(const State& right)
{
	if (this->runningState != right.runningState)
	{
		return this->runningState > right.runningState;
	}
	else
	{
		return this->operatingStatus > this->operatingStatus;
	}
}

/*是否小于right状态*/
bool StationInfo::State::operator<(const State& right)
{
	if (this->runningState != right.runningState)
	{
		return this->runningState < right.runningState;
	}
	else
	{
		return this->operatingStatus < this->operatingStatus;
	}
}

/*!
设置操作状态和附加消息
@param const OperatingStatus & status 操作状态
@param QString attachMessage 附加消息
@return void
作者：cokkiy（张立民)
创建时间：2015/12/04 9:10:31
*/
void StationInfo::State::setStatus(const StationInfo::OperatingStatus& status, QString attachMessage /*= QStringLiteral("")*/)
{
    setOperatingStatus(status);
    this->attachMessage = attachMessage;
}

/*!
设置附加消息
@param QString message 附加消息
@return void
作者：cokkiy（张立民)
创建时间：2015/12/04 10:37:55
*/
void StationInfo::State::setAttachMessage(QString message)
{
    this->attachMessage = message;
}

/*!
获取代表当前状态（包括运行和操作)的字符串
@return QString 代表当前状态（包括运行和操作)的字符串
作者：cokkiy（张立民)
创建时间：2015/12/03 22:14:30
*/
QString StationInfo::State::toString()
{
    QString result = QStringLiteral("%1 %2 %3");
    return result.arg(translate(runningState)).arg(translate(operatingStatus)).arg(attachMessage);
}


/*!
获取当前运行状态
@return StationInfo::RunningState 当前运行状态
作者：cokkiy（张立民)
创建时间：2015/12/04 9:24:57
*/
StationInfo::RunningState StationInfo::State::getRunningState()
{
    return runningState;
}

/*!
获取当前操作状态
@return StationInfo::RunningState 当前操作状态
作者：cokkiy（张立民)
创建时间：2015/12/04 9:24:57
*/
StationInfo::OperatingStatus StationInfo::State::getOperatingStatus()
{
    return operatingStatus;
}

/*!
是否在关机或重启状态
@return bool
作者：cokkiy（张立民)
创建时间：2015/12/16 17:01:53
*/
bool StationInfo::inRebootingOrShutdown()
{
    return m_state == Shutdowning || m_state == Rebooting;
}

/*
 设置操作状态.同时切换运行状态
 **/
void StationInfo::State::setOperatingStatus(const OperatingStatus& opStatus)
{
    this->operatingStatus = opStatus;
    switch (opStatus)
    {
    case StationInfo::PowerOffOrNetworkFailure:
        runningState = Unknown;
        break;
    case StationInfo::NoHeartbeat:
        runningState = Warning;
        break;
    case StationInfo::AppStarted:
    case StationInfo::Running:
        runningState = Normal;
        break;
    case StationInfo::Powering:
        runningState = Unknown;
        break;
    case StationInfo::Shutdowning:
    case StationInfo::Rebooting:
        runningState = Warning;
        break;

    case StationInfo::GeneralError:
    case StationInfo::AppStartFailure:
    case StationInfo::AppCloseFailure:
    case StationInfo::PowerOnFailure:
    case StationInfo::ShutdownFailure:
    case StationInfo::RebootFailure:
        runningState = Error;
        break;
    case StationInfo::CanNotOperating:
    case StationInfo::SomeAppNotRunning:
    case StationInfo::CPUTooHigh:
    case StationInfo::MemoryTooHigh:
    case StationInfo::DiskFull:
    case  StationInfo::AppNotRunning:
        runningState = Warning;
        break;
    default:
        break;
    }
}

/*翻译运行状态*/
QString StationInfo::State::translate(RunningState state)
{
    QString result = QStringLiteral("");
    switch (state)
    {
    case StationInfo::Unknown:
        result = tr("Unknown");
        break;
    case StationInfo::Normal:
        result = tr("Normal");
        break;
    case StationInfo::Warning:
        result = tr("Warning");
        break;
    case StationInfo::Error:
        result = tr("Error");
        break;
    default:
        break;
    }
    return result;
}
/*翻译操作状态*/
QString StationInfo::State::translate(OperatingStatus status)
{
    QString result = QStringLiteral("");
    switch (status)
    {
    case StationInfo::PowerOffOrNetworkFailure:
        result = tr("PowerOffOrNetworkFailure");
        break;
    case StationInfo::NoHeartbeat:
        result = tr("NoHeartbeat");
        break;
    case StationInfo::Powering:
        result = tr("Powering");
        break;
    case StationInfo::AppStarting:
        result = tr("AppStarting");
        break;
    case StationInfo::Shutdowning:
        result = tr("Shutdowning");
        break;
    case StationInfo::Rebooting:
        result = tr("Rebooting");
        break;
    case StationInfo::PowerOnFailure:
        result = tr("PowerOnFailure");
        break;
    case StationInfo::AppCloseFailure:
        result = tr("AppCloseFailure");
        break;
    case StationInfo::AppStartFailure:
        result = tr("AppStartFailure");
        break;
    case StationInfo::AppStarted:
        result = tr("AppStarted");
        break;
    case StationInfo::AppNotRunning:
        result = tr("AppNotRunning");
        break;
    case StationInfo::SomeAppNotRunning:
        result = tr("SomeAppNotRunning");
        break;
    case StationInfo::ShutdownFailure:
        result = tr("ShutdownFailure");
        break;
    case StationInfo::RebootFailure:
        result = tr("RebootFailure");
        break;
    case StationInfo::MemoryTooHigh:
        result = tr("MemoryTooHigh");
        break;
    case StationInfo::CPUTooHigh:
        result = tr("CPUTooHigh");
        break;
    case StationInfo::DiskFull:
        result = tr("DiskFull");
        break;
    case StationInfo::Running:
        result = tr("Running");
        break;
    case StationInfo::GeneralError:
        result = tr("GeneralError");
        break;
    case StationInfo::CanNotOperating:
        result = tr("CanNotOperating");
        break;
    default:
        break;
    }

    return result;
}

/*!
返回表示当前状态的Html字符串
@return QString 表示当前状态的Html字符串
作者：cokkiy（张立民)
创建时间：2015/12/11 16:08:49
*/
QString StationInfo::State::toHtmlString()
{
    switch (runningState)
    {
    case StationInfo::Unknown:
        return QStringLiteral("<span class=\"Unknown\">%1/</span>").arg(this->toString());
        break;
    case StationInfo::Normal:
        return QStringLiteral("<span class=\"Normal\">%1/</span>").arg(this->toString());
        break;
    case StationInfo::Warning:
        return QStringLiteral("<span class=\"Warning\">%1/</span>").arg(this->toString());
        break;
    case StationInfo::Error:
        return QStringLiteral("<span class=\"Error\">%1/</span>").arg(this->toString());
        break;
    default:
        break;
    }
    return QStringLiteral("");
}
