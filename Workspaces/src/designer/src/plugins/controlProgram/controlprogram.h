#ifndef CONTROLPROGRAM_H
#define CONTROLPROGRAM_H

#include<QHeaderView>
#include<QTableWidget>
#include<Net/NetComponents>

class controlProgram : public QTableWidget
{
    Q_OBJECT
    Q_PROPERTY(QString dataes READ getDataes WRITE setDataes)//多参数
    Q_PROPERTY(QStringList nameList READ nameList WRITE setNameList)//字符串列表

    //all
    Q_PROPERTY(int percent READ getPercent WRITE setPercent)// COLUMN PERCENT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)//背景色

    //coulmn = 0 测试项目列
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)

    //coulmn = 1 状态列 三种状态的字体颜色
    Q_PROPERTY(QFont undoFont READ getUndoFont WRITE setUndoFont)
    Q_PROPERTY(QColor undoColor READ getUndoColor WRITE setUndoColor)
    Q_PROPERTY(QFont doingFont READ getDoingFont WRITE setDoingFont)
    Q_PROPERTY(QColor doingColor READ getDoingColor WRITE setDoingColor)
    Q_PROPERTY(QFont doneFont READ getDoneFont WRITE setDoneFont)
    Q_PROPERTY(QColor doneColor READ getDoneColor WRITE setDoneColor)

    //grid
    Q_PROPERTY(QColor lineColor READ lineColor WRITE
               setGridlineColor NOTIFY gridlineColorChanged)//网格线条颜色

    //图元坐标
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)
    //设置图元大小策略的属性
    Q_PROPERTY(QSizePolicy sizePolicy READ sizePolicy WRITE setSizePolicy)
    Q_PROPERTY(QSize minimumSize READ minimumSize WRITE setMinimumSize)
    Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize)
    Q_PROPERTY(QSize sizeIncrement READ sizeIncrement WRITE setSizeIncrement)
    Q_PROPERTY(QSize baseSize READ baseSize WRITE setBaseSize)

public:
    controlProgram(QWidget *parent = 0);
    virtual ~controlProgram();

    int getPercent() const{return m_percent;}
    void setPercent(const int &newp);

    QString getDataes() const{return m_dataes;}
    void setDataes(const QString &newdataes);

    QStringList nameList() const { return m_nameList; }
    void setNameList(const QStringList &newnameList);

    QFont getUndoFont() const{return m_undofont;}
    void setUndoFont(const QFont &newfont);

    QFont getDoingFont() const{return m_doingfont;}
    void setDoingFont(const QFont &newfont);

    QFont getDoneFont() const{return m_donefont;}
    void setDoneFont(const QFont &newfont);

    QColor getUndoColor() const{return m_undoColor;}
    void setUndoColor(const QColor &newcolor);

    QColor getDoingColor() const{return m_doingColor;}
    void setDoingColor(const QColor &newcolor);

    QColor getDoneColor() const{return m_doneColor;}
    void setDoneColor(const QColor &newcolor);

    QColor textColor() const{return m_textColor;}
    void setTextColor(const QColor &newcolor);

    QColor backgroundColor() const{return m_backgroundColor;}
    void setBackgroundColor(const QColor &newcolor);

    QColor lineColor() const{return m_gridlineColor;}
    void setGridlineColor(const QColor &newcolor);

private:    
    QString m_dataes;
    int intdata;
    QStringList datalist1,datalist2;//用于存储上一帧数据和本帧数据
    QColor m_backgroundColor;//背景色
    QColor m_textColor,m_gridlineColor;//第一列文本颜色以及网格颜色
    QColor m_undoColor,m_doingColor,m_doneColor;//文本颜色
    QFont m_undofont,m_doingfont,m_donefont;//字体
    QStringList m_nameList,stateList,titleList;//存储字符串
    int m_percent;//百分比

    QStringList m_FormulaVector;//用于存储解释的多参数

    //用于判断是否更新grid颜色
    bool hasStyleSheetUpdate;
    bool hashandlename;
    bool haspercent;

    //定时器类
    TimerInterface* m_ti;
    DataCenterInterface* m_dc;
    //用于获取参数信息
    InformationInterface* net;
    AbstractParam* param;

public slots:
    void timeEvent_500ms();

signals:
    void gridlineColorChanged(QColor);

protected:
    void paintEvent(QPaintEvent *e);

private slots:
    void handleName();//用于重写表头及第一列
    void handleData(int rows,int columns);//用于重写第二列
    void loadData();//导入数据
    void stylesheetUpdate();//用于更新grid颜色
};

#endif
