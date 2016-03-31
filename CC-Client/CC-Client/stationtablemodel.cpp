#include "stationtablemodel.h"
#include <QIcon>
#include "QBrush"
/*!
创建\see StationTableModel对象实例
@param QObject * parent 父对象
@param const StationList* pStations 工作站列表
@return StationTableModel对象实例
作者：cokkiy（张立民)
创建时间：2015/11/09 9:21:44
*/
StationTableModel::StationTableModel(const StationList* pStations, QObject *parent)
    : QAbstractTableModel(parent)
{
    this->pStations = pStations;
    const_cast<StationList*>(pStations)->subscribeAllStationsPropertyChangedEvent(this, SLOT(stationPropertyChanged(const QString&, const QObject* )));
    connect(pStations, SIGNAL(stationListChanged()), this, SLOT(stationListChanged()));
    connect(pStations, SIGNAL(stationAdded(StationInfo* )), this, SLOT(stationAdded(StationInfo* )));
    powerOnIcons[0].addFile(":/Icons/powerOn-0.png");
    powerOnIcons[1].addFile(":/Icons/powerOn-1.png");
    powerOnIcons[2].addFile(":/Icons/powerOn-2.png");
    powerOnIcons[3].addFile(":/Icons/powerOn-3.png");
    startCloseAppIcons[0].addFile(":/Icons/startApp0.png");
    startCloseAppIcons[1].addFile(":/Icons/startApp1.png");
    shutdownRebootIcons[0].addFile(":/Icons/shutdown0.png");
    shutdownRebootIcons[1].addFile(":/Icons/shutdown1.png");
    shutdownRebootIcons[2].addFile(":/Icons/shutdown2.png");
    shutdownRebootIcons[3].addFile(":/Icons/shutdown3.png");
    shutdownRebootIcons[4].addFile(":/Icons/shutdown4.png");
}

StationTableModel::~StationTableModel()
{

}
//返回行数
int StationTableModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return pStations->currentCount();
}
//返回列数
int StationTableModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return totalColumnCount;
}
//view 获取数据方法
QVariant StationTableModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= pStations->currentCount())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        if (displayMode==List)
        {
            QString str = QStringLiteral("名称：%1\tIP：%2\t状态：%3\t内存(%)：%4\tCPU：%5");
            StationInfo* s = pStations->atCurrent(index.row());
            QString memory = QStringLiteral("%1").arg(pStations->atCurrent(index.row())->Memory() / pStations->atCurrent(index.row())->TotalMemory() * 100, 0, 'f', 2);
            return str.arg(s->Name()).arg(s->IP()).arg(s->state().toString()).arg(memory).arg(s->CPU());
        }
        else if(displayMode==Details)
        {
            return getColumnValue(index);
        }
        else
        {
            return pStations->atCurrent(index.row())->Name();
        }        
    }

    if (role == Qt::TextAlignmentRole)
    {
        if (displayMode == LargerIcons || displayMode == SmallIcons)
        {
            return Qt::AlignHCenter;
        }
        else
        {
            return Qt::AlignLeft;
        }
    }

    if (role == Qt::ToolTipRole)
    {
        QString tip = QStringLiteral("%1\r\nIP：%2\r\n%3");
        auto s = pStations->atCurrent(index.row());
        return tip.arg(s->Name()).arg(s->IP()).arg(s->state().toString());
    }
    if (role == Qt::ForegroundRole)
    {
        QVariant v;
        switch (pStations->atCurrent(index.row())->state().getRunningState())
        {
        case StationInfo::Normal:
        {
            QBrush brush(QColor(0, 85, 0));
            v.setValue(brush);
        }
        break;
        case  StationInfo::Warning:
        {
            QBrush brush(QColor(208, 139, 0));
            v.setValue(brush);
        }
        break;
        case  StationInfo::Error:
        {
            QBrush brush(QColor(170, 0, 0));
            v.setValue(brush);
        }
        break;
        default:
            break;
        }
        return v;
    }
    
    else if (role == Qt::DecorationRole)
    {
        if(index.column()==0)
            return getIcon(index);
    }

    return QVariant();
}

void StationTableModel::sort(int column, Qt::SortOrder order /*= Qt::AscendingOrder*/)
{

}

void StationTableModel::beginFilter()
{
	beginResetModel();
	
}

void StationTableModel::endFilter()
{
	endResetModel();
}

//view获取行标题和列标题
QVariant StationTableModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return QStringLiteral("名称");
        case 1:
            return QStringLiteral("IP");
        case 2:
            return QStringLiteral("状态");        
        case 3:
            return QStringLiteral("总CPU占用率(%)");
        case 4:
            return QStringLiteral("总内存占用率(%)");
        case 5:
            return QStringLiteral("指显CPU占用率");
        case 6:
            return QStringLiteral("指显内存占用率");
        case 7:
            return QStringLiteral("总内存");
        case 8:
            return QStringLiteral("总进程数");
        case 9:
            return QStringLiteral("指显线程数");
        case 10:
            return QStringLiteral("MAC地址");
        default:
            break;
        }
    }
    else
        return section;
    return section;
}

/*!
设置显示模式
@param DisplayMode mode  显示模式
@return void
作者：cokkiy（张立民)
创建时间：2015/11/06 17:39:01
*/
void StationTableModel::setDisplayMode(DisplayMode mode /*= DisplayMode::SmallIcons*/)
{
    this->displayMode = mode;
}

//获取图标
QVariant StationTableModel::getIcon(const QModelIndex &index) const
{
    QVariant v;
    if (displayMode == LargerIcons)
    {
        //大图标
        QIcon icon;
        switch (pStations->atCurrent(index.row())->state().getRunningState())
        {
        case StationInfo::Unknown:
            icon.addFile(QStringLiteral(":/Icons/ncom002.ico"), QSize(64, 64), QIcon::Normal, QIcon::On);
            break;
        case StationInfo::Normal:
            icon.addFile(QStringLiteral(":/Icons/ncom003.ico"), QSize(64, 64), QIcon::Normal, QIcon::On);
            break;
        case  StationInfo::Warning:
            icon.addFile(QStringLiteral(":/Icons/ncom008.ico"), QSize(64, 64), QIcon::Normal, QIcon::On);
            break;
        case  StationInfo::Error:
            icon.addFile(QStringLiteral(":/Icons/ncom006.ico"), QSize(64, 64), QIcon::Normal, QIcon::On);
            break;
        }
        switch (pStations->atCurrent(index.row())->state().getOperatingStatus())
        {       
        case  StationInfo::Powering:           
            return powerOnIcons[pStations->atCurrent(index.row())->ExecuteCounting() % 4];
            break;
        case  StationInfo::AppStarting:
            return startCloseAppIcons[pStations->atCurrent(index.row())->ExecuteCounting() % 2];
            break;
        case StationInfo::Rebooting:
        case StationInfo::Shutdowning:
            return shutdownRebootIcons[pStations->atCurrent(index.row())->ExecuteCounting() % 5];
            break;
        }
        v.setValue(icon);
    }
    else
    {
        //小图标
        QIcon icon;
        switch (pStations->atCurrent(index.row())->state().getRunningState())
        {
        case StationInfo::Unknown:
            icon.addFile(QStringLiteral(":/Icons/ncom002.ico"), QSize(128, 128), QIcon::Normal, QIcon::On);
            break;
        case StationInfo::Normal:
            icon.addFile(QStringLiteral(":/Icons/ncom003.ico"), QSize(128, 128), QIcon::Normal, QIcon::On);
            break;
        case  StationInfo::Warning:
            icon.addFile(QStringLiteral(":/Icons/ncom008.ico"), QSize(128, 128), QIcon::Normal, QIcon::On);
            break;
        case  StationInfo::Error:
            icon.addFile(QStringLiteral(":/Icons/ncom006.ico"), QSize(128, 128), QIcon::Normal, QIcon::On);
            break;
        }
        switch (pStations->atCurrent(index.row())->state().getOperatingStatus())
        {
        case  StationInfo::Powering:
            return powerOnIcons[pStations->atCurrent(index.row())->ExecuteCounting() % 4];
            break;
        case  StationInfo::AppStarting:
            return startCloseAppIcons[pStations->atCurrent(index.row())->ExecuteCounting() % 2];
            break;
        case StationInfo::Rebooting:
        case StationInfo::Shutdowning:
            return shutdownRebootIcons[pStations->atCurrent(index.row())->ExecuteCounting() % 5];
            break;
        }
        v.setValue(icon);
    }
    return v;
}

QVariant StationTableModel::getColumnValue(const QModelIndex &index) const
{
    switch (index.column())
    {
    case 0:
        return pStations->atCurrent(index.row())->Name();
        break;
    case 1:
        return pStations->atCurrent(index.row())->IP();
    case 2:
        return pStations->atCurrent(index.row())->state().toString();
    case 3:
        return QStringLiteral("%1").arg(pStations->atCurrent(index.row())->CPU(), 0, 'f', 0);
    case 4:
        return QStringLiteral("%1").arg(pStations->atCurrent(index.row())->Memory() / pStations->atCurrent(index.row())->TotalMemory() * 100, 0, 'f', 2);
    case 5:
        return QStringLiteral("%1").arg(pStations->atCurrent(index.row())->AppCPU(), 0, 'f', 0);
    case 6:
        return QStringLiteral("%1").arg(pStations->atCurrent(index.row())->AppMemory(), 0, 'f', 0);
    case 7:
        return QStringLiteral("%1").arg(pStations->atCurrent(index.row())->TotalMemory());
    case 8:
        return QStringLiteral("%1").arg(pStations->atCurrent(index.row())->ProcCount());
    case 9:
        return QStringLiteral("%1").arg(pStations->atCurrent(index.row())->AppThreadCount());
    case 10:
        return pStations->atCurrent(index.row())->MAC();
    default:
        break;
    }    
    return QVariant();
}

//工作站状态发生变化
void StationTableModel::stationPropertyChanged(const QString& propertyName, const QObject* station)
{
    int row = pStations->indexAtCurrent((StationInfo*)station);
    if (row != -1)
    {
        dataChanged(index(row, 0), index(row, totalColumnCount));
    }
}

void StationTableModel::stationListChanged()
{
    this->beginResetModel();
    this->endResetModel();
}
//工作站被添加
void StationTableModel::stationAdded(StationInfo* addedStation)
{
    addedStation->subscribePropertyChanged(this, SLOT(stationPropertyChanged(const QString&, const QObject*)));
}
