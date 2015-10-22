#ifndef CONTROLPROGRAM_H
#define CONTROLPROGRAM_H

#include<QList>
#include<QComboBox>
#include<QHeaderView>
#include<QTableView>
#include<QTableWidget>
#include<Net/NetComponents>

class controlProgram : public QTableWidget
{
    Q_OBJECT
    Q_PROPERTY(QString dataes READ getDataes WRITE setDataes NOTIFY dataesChanged)//多参数
    Q_PROPERTY(QStringList nameList READ nameList WRITE setNameList NOTIFY nameListChanged)//字符串列表
    //all
    Q_PROPERTY(int percent READ getPercent WRITE setPercent)// COLUMN PERCENT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)//背景色
    //    Q_PROPERTY(Shape frameShape READ frameShape WRITE setFrameShape)//边框三维效果
    //    Q_PROPERTY(Shadow frameShadow READ frameShadow WRITE setFrameShadow)
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth) //边框宽度
    //coulmn = 0 测试项目列
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
    //coulmn = 1 状态列 三种状态的字体颜色
    Q_PROPERTY(QFont undofont READ getUndoFont WRITE setUndoFont)
    Q_PROPERTY(QColor undoColor READ getUndoColor WRITE setUndoColor)
    Q_PROPERTY(QFont doingfont READ getDoingFont WRITE setDoingFont)
    Q_PROPERTY(QColor doingColor READ getDoingColor WRITE setDoingColor)
    Q_PROPERTY(QFont donefont READ getDoneFont WRITE setDoneFont)
    Q_PROPERTY(QColor doneColor READ getDoneColor WRITE setDoneColor)
    //grid
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid)//网格
    Q_PROPERTY(Qt::PenStyle gridStyle READ gridStyle WRITE setGridStyle)//网格样式
    Q_PROPERTY(QColor gridlineColor READ gridlineColor WRITE
               setGridlineColor NOTIFY gridlineColorChanged)//网格线条颜色
    //图元坐标
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)
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

    QColor gridlineColor() const{return m_gridlineColor;}
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
    QComboBox *comboBox;//用于添加字符串列表
    bool hasStyleSheetUpdate;//用于判断是否更新grid颜色

    //定时器 用于更新图形绘制
    int m_timer_id;//
    DataCenterInterface* m_dc;//
    //用于获取参数信息
    InformationInterface* net;
    AbstractParam* param;

signals:
    void dataesChanged(QString);
    void nameListChanged(QStringList);
    void gridlineColorChanged(QColor);

protected:
    void timerEvent1(QTimerEvent *e);
    void paintEvent(QPaintEvent *e);

private slots:
//    void handleName();//用于重写表头及第一列
    void handleData(int rows,int columns);//用于重写第二列
    void loadData();//导入数据
    void setNameBox();//字符串列表
    void stylesheetUpdate();//用于更新grid颜色
};

#endif
