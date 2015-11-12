#pragma once
#include <QString>
#include <QMetaType>
#include <QObject>
#include <QVariant>
/**************************************
 @class: StationInfo 指显工作站信息类,定义了指显工作站
 @see IP, @see MAC @see Name
 **************************************/
class StationInfo :QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state WRITE setState)
    Q_PROPERTY(float CPU READ CPU WRITE setCPU)
    Q_PROPERTY(float Memory READ Memory WRITE setMemory)
    Q_PROPERTY(float ZXCPU READ ZXCPU WRITE setZXCPU)
    Q_PROPERTY(int ProcCount READ ProcCount WRITE setProcCount)
    Q_PROPERTY(int ZXThreadCount READ ZXThreadCount WRITE setZXThreadCount)
    Q_PROPERTY(size_t TotalMemory READ TotalMemory WRITE setTotalMemory)
    Q_PROPERTY(float ZXMemory READ ZXMemory WRITE setZXMemory)
    Q_PROPERTY(bool ZXIsRunning READ ZXIsRunning WRITE setZXIsRunning)
    Q_PROPERTY(QString hint READ hint WRITE setHint)

public:
    //工作站状态
    enum State
    {
        Unkonown = 0,/*未知*/
        Normal = 1,/*正常*/
        Warning,/*告警*/
        Error,/*异常*/
        Powering, /*加电中...*/
        AppStarting,/*指显软件启动中*/
    };
private:
    bool m_ZXIsRunning = false;
public:
    bool ZXIsRunning()
    {
        return m_ZXIsRunning;
    }
    void setZXIsRunning(bool value)
    {
        if (m_ZXIsRunning != value)
        {
            m_ZXIsRunning = value;
            emit propertyChanged("ZXIsRunning", this);
        }
    }

private:
    QString m_hint;
public:
    QString hint()
    {
        return m_hint;
    }
    void setHint(QString value)
    {
        if (m_hint != value)
        {
            m_hint = value;
            emit propertyChanged("hint", this);
        }
    }


private:
    float m_ZXMemory;
public:
    float ZXMemory()
    {
        return m_ZXMemory;
    }
    void setZXMemory(float value)
    {
        if (m_ZXMemory != value)
        {
            m_ZXMemory = value;
            emit propertyChanged("ZXMemory", this);
        }
    }


private:
    size_t m_TotalMemory;
public:
    size_t TotalMemory()
    {
        return m_TotalMemory;
    }
    void setTotalMemory(size_t value)
    {
        if (m_TotalMemory != value)
        {
            m_TotalMemory = value;
            emit propertyChanged("TotalMemory", this);
        }
    }


private:
    int m_zxThreadCount;
public:
    int ZXThreadCount()
    {
        return m_zxThreadCount;
    }
    void setZXThreadCount(int value)
    {
        if (m_zxThreadCount != value)
        {
            m_zxThreadCount = value;
            emit propertyChanged("ZXThreadCount", this);
        }
    }


private:
    int m_ProcCount;
public:
    int ProcCount()
    {
        return m_ProcCount;
    }
    void setProcCount(int value)
    {
        if (m_ProcCount != value)
        {
            m_ProcCount = value;
            emit propertyChanged("ProcCount", this);
        }
    }


private:
    float m_zxCPU;
public:
    float ZXCPU()
    {
        return m_zxCPU;
    }
    void setZXCPU(float value)
    {
        if (m_zxCPU != value)
        {
            m_zxCPU = value;
            emit propertyChanged("ZXCPU", this);
        }
    }

private:
    float m_Memory;
public:
    float Memory()
    {
        return m_Memory;
    }
    void setMemory(float value)
    {
        if (m_Memory != value)
        {
            m_Memory = value;
            emit propertyChanged("Memory", this);
        }
    }

private:
    State m_state = Unkonown;
public:
    State state()
    {
        return m_state;
    }
    void setState(State value)
    {
        if (m_state != value)
        {
            m_state = value;
            emit propertyChanged("state", this);
        }
    }

private:
    float m_CPU;
public:
    float CPU()
    {
        return m_CPU;
    }
    void setCPU(float value)
    {
        if (m_CPU != value)
        {
            m_CPU = value;
            emit propertyChanged("CPU", this);
        }
    }

    /*默认构造函数*/
    StationInfo() = default;
    StationInfo(const StationInfo& ref);
    /*!
    返回表示状态的字符
    @return QString \see State表示状态的字符
    作者：cokkiy（张立民)
    创建时间：2015/11/06 16:22:51
    */
    QString state2String();

    /*
    工作站名称
    **/
    QString name;
    /*
    工作站MAC地址
    **/
    QString mac;
    /*
     工作站IP地址
     **/
    QString IP;

public:
    /*!
    订阅属性变化事件
    @param const QObject * receiver 接收者
    @param const char * member 接收者处理函数
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/12 9:35:22
    */
    void subscribePropertyChanged(const QObject* receiver, const char* member);

signals:
    /*!
    属性发生变化
    @param QString propertyName 属性名称
    @param QVariant vlaue 属性值
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/11 21:25:59
    */
    void propertyChanged(const QString& propertyName, const QObject* owner);
};

//注册到metatype中
Q_DECLARE_METATYPE(StationInfo::State)
Q_DECLARE_METATYPE(StationInfo)
