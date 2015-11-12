#ifndef STATIONTABLEMODEL_H
#define STATIONTABLEMODEL_H

#include <QAbstractTableModel>
#include "StationList.h"
#include <QVariant>
/**************************************************************************
* StationTableModel: Model of Station Table, 实现M/V模式的Model
**************************************************************************/
class StationTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /*显示模式*/
    enum DisplayMode
    {
        SmallIcons = 0,/*小图标*/
        LargerIcons,/*大图标*/
        List,/*列表*/
        Details,/*详细信息*/
    };
    /*!
    创建\see StationTableModel对象实例
    @param QObject * parent 父对象
    @param const StationList* pStations 指向工作站列表的指针
    @return StationTableModel对象实例
    作者：cokkiy（张立民)
    创建时间：2015/11/09 9:21:44
    */
    StationTableModel(const StationList* pStations, QObject *parent = NULL);
    /*析构函数,释放资源*/
    ~StationTableModel();

    //view获取行数的方法
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    //view获取列数的方法
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    //view获取数据的方法
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    //排序方法
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    //view获取行标题和列标题
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /*!
    设置显示模式
    @param DisplayMode mode  显示模式
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/06 17:39:01
    */
    void setDisplayMode(DisplayMode mode = DisplayMode::SmallIcons);
private:
    //工作站列表
    const StationList* pStations;
    //获取图标
    QVariant getIcon(const QModelIndex &index) const;
    //获取列值
    QVariant getColumnValue(const QModelIndex &index) const;
    //显示模式
    DisplayMode displayMode = SmallIcons;
    //图标索引
    int icon_index = 0;

    private slots:
    //工作站状态发生变化
    void stationPropertyChanged(const QString& propertyName, const QObject* station);
};

#endif // STATIONTABLEMODEL_H
