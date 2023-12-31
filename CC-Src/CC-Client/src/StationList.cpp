﻿#include "StationList.h"
#include <QFile>
#include <QRegExp>
#include <QTextCodec>
#include <QTextStream>
#include <QValidator>

/*!
获取当前显示工作站数量
@return int 工作站数量
作者：cokkiy（张立民)
创建时间：2015/11/04 12:06:18
*/
int StationList::currentCount() const { return currentStations.size(); }

/*!
获取全部工作站数量
@return int 全部工作站数量
作者：cokkiy（张立民)
创建时间：2015/11/12 17:13:55
*/
int StationList::total() { return allStations.size(); }

/*!
获取当前工作站显示列表（过滤后的)中\see row位置的工作站
@param int row 指定位置
@return StationInfo \see row位置的工作站信息
作者：cokkiy（张立民)
创建时间：2015/11/04 12:07:09
*/
StationInfo *StationList::atCurrent(int row) { return currentStations[row]; }

/*!
获取当前工作站显示列表（过滤后的)\see row位置的工作站引用
@param int row 指定位置
@return StationInfo \see row位置的工作站引用
作者：cokkiy（张立民)
创建时间：2015/11/04 12:07:09
*/
StationInfo &StationList::refCurrent(int row) { return *currentStations[row]; }

/*!
获取当前工作站显示列表（过滤后的)中\see row位置的工作站(const)
@param int row 指定位置
@return StationInfo \see row位置的工作站信息
作者：cokkiy（张立民)
创建时间：2015/11/10 21:55:38
*/
StationInfo *StationList::atCurrent(int row) const {
  return currentStations[row];
}

/*!
获取指定工作站在当前显示列表中的位置
@param StationInfo * pStation 工作站
@return int
作者：cokkiy（张立民)
创建时间：2015/11/12 10:50:06
*/
int StationList::indexAtCurrent(const StationInfo *pStation) const {
  int index = -1;
  bool isExists = false;
  for (auto p : currentStations) {
    index++; // first is 0
    if (pStation == p) {
      isExists = true;
      break;
    }
  }
  return isExists ? index : -1;
}

/*!
查找指定IP的工作站信息
@param QString ip 工作站IP
@return StationInfo* 指定IP的工作站指针
作者：cokkiy（张立民)
创建时间：2015/11/12 17:41:43
*/
StationInfo *StationList::find(QString ip) {
  for (auto &s : allStations) {
    QStringList ipList = s.IP().split(",", QString::SkipEmptyParts);
    if (ipList.indexOf(ip) != -1) {
      return &s;
    }
  }
  return NULL;
}

/*!
查找指定mac->IP对应的工作站信息
@param std::pair<const std::string, std::list<std::string>>
工作站的mac->list<ip>
@return StationInfo* 指定信息的工作站指针,如果不存在,返回NULL
作者：cokkiy（张立民)
创建时间：2015/11/24 10:22:10
*/
StationInfo *StationList::find(
    const std::pair<const std::string, std::list<std::string>> &ni) {
  for (auto &station : allStations) {
    for (auto &tempNI : station.NetworkIntefaces) {
      if (tempNI.hasMacAndIPConfig(ni)) {
        return &station;
      }
    }
  }

  return NULL;
}

/*!
订阅全部工作站属性变化通知事件
@param const QObject * receiver 事件接收函数所在对象的指针
@param const char * member 事件接收函数名称
@return void
作者：cokkiy（张立民)
创建时间：2015/11/12 17:44:28
*/
void StationList::subscribeAllStationsPropertyChangedEvent(
    const QObject *receiver, const char *member) {
  for (auto &station : allStations) {
    station.subscribePropertyChanged(receiver, member);
  }
}

/*!
订阅全部工作站状态变化通知事件
@param const QObject * receiver 事件接收函数所在对象的指针
@param const char * member 事件接收函数名称
@return void
作者：cokkiy（张立民)
创建时间：2015/11/13 10:57:28
*/
void StationList::subscribeAllStationsStateChangedEvent(const QObject *receiver,
                                                        const char *member) {
  for (auto &station : allStations) {
    station.subscribeStateChanged(receiver, member);
  }
}

/*!
订阅全部工作站用户编辑导致的属性变化通知事件
@param const QObject * receiver 事件接收函数所在对象的指针
@param const char * member 事件接收函数名称
@return void
作者：cokkiy（张立民)
创建时间：2015/11/13 10:57:28
*/
void StationList::subscribeAllStationsChangedEvent(const QObject *receiver,
                                                   const char *member) {
  for (auto &station : allStations) {
    station.subscribeStationChanged(receiver, member);
  }
}

/*!
订阅工作站被添加事件
@param const QObject * receiver 事件接收函数所在对象的指针
@param const char * member 事件接收函数名称
@return void
作者：cokkiy（张立民)
创建时间：2015/12/02 18:01:41
*/
void StationList::subscribeStationAddedEvent(const QObject *receiver,
                                             const char *member) {
  connect(this, SIGNAL(stationAdded(StationInfo *)), receiver, member);
}

/*!
订阅工作站列表变化事件
@param const QObject * receiver 事件接收函数所在对象的指针
@param const char * member 事件接收函数名称
@return void
作者：cokkiy（张立民)
创建时间：2015/12/03 9:14:33
*/
void StationList::subscribeListChangedEvent(const QObject *receiver,
                                            const char *member) {
  connect(this, SIGNAL(stationListChanged()), receiver, member);
}

/*!
根据@param filterString过滤工作站
@param const FilterOperations& filter 过滤条件
@return void
作者：cokkiy（张立民)
创建时间：2015/11/04 12:29:36
*/
void StationList::filter(const FilterOperations &filter) {
  this->filterOperations = filter;
}

/*!
根据filter字符串过滤工作站列表
@param const QString filter 过滤字符串，只支持IP和名称
@return void
作者：cokkiy（张立民）
创建时间:2016/3/30 10:33:02
*/
void StationList::filter(const QString &strFilter) {
  QRegExp ipReg("^((?:(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(?:"
                "25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d))))&");
  QRegExpValidator validator;
  validator.setRegExp(ipReg);
  QString s = strFilter;
  int pos = 0;
  currentStations.clear();
  if (validator.validate(s, pos) == QValidator::Intermediate) {
    // IP地址
    for (auto &station : allStations) {
      if (station.IP().contains(s)) {
        currentStations.push_back(&station);
      }
    }
  } else {
    //名称
    for (auto &station : allStations) {
      if (station.Name().contains(s)) {
        currentStations.push_back(&station);
      }
    }
  }
}

/*!
根据\param sortby排序工作站列表
@param \see SortBy sortby 排序方式
@return void
作者：cokkiy（张立民)
创建时间：2015/11/04 12:32:52
*/
void StationList::sort(SortBy sortby /*= SortBy::IP*/) {
  this->sortby = sortby;
  switch (sortby) {
  case StationList::IP:
    //根据IP正排序
    std::sort(currentStations.begin(), currentStations.end(),
              [](StationInfo *first, StationInfo *second) {
                return first->IP() < second->IP();
              });
    break;
  case StationList::IP_DESC:
    //根据IP逆排序
    std::sort(currentStations.begin(), currentStations.end(),
              [](StationInfo *first, StationInfo *second) {
                return first->IP() >= second->IP();
              });
    break;
  case StationList::State:
    //根据状态正排序
    std::sort(currentStations.begin(), currentStations.end(),
              [](StationInfo *first, StationInfo *second) {
                return first->state() >= second->state();
              });
    break;
  case StationList::State_DESC:
    //根据状态逆排序
    std::sort(currentStations.begin(), currentStations.end(),
              [](StationInfo *first, StationInfo *second) {
                return first->state() < second->state();
              });
    break;
  case StationList::Name:
    //根据名称正排序
    std::sort(currentStations.begin(), currentStations.end(),
              [](StationInfo *first, StationInfo *second) {
                return first->Name() >= second->Name();
              });
    break;
  case StationList::Name_DESC:
    //根据名称逆排序
    std::sort(currentStations.begin(), currentStations.end(),
              [](StationInfo *first, StationInfo *second) {
                return first->Name() < second->Name();
              });
    break;
  default:
    //不排序
    break;
  }
}

/*!
从列表中删除指定的工作站
@param StationInfo * s 工作站指针
@return void
作者：cokkiy（张立民)
创建时间：2015/12/03 15:44:09
*/
void StationList::remove(const StationInfo *station) {
  // First remove from current list
  for (auto iter = currentStations.begin(); iter != currentStations.end();
       iter++) {
    if (*iter == station) {
      currentStations.erase(iter);
      break;
    }
  }
  // remove from all list
  // allStations.remove(*station);

  //发生消息
  emit stationListChanged();
}

/*!
把工作站列表保存到文件中
@param QString fileName 文件名
@return void
作者：cokkiy（张立民)
创建时间：2015/12/02 21:32:36
*/
void StationList::saveToFile(QString fileName) {
  QFile file(fileName);
  if (file.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream config(&file);
    config.setCodec(QTextCodec::codecForName("UTF-8"));
    config << QStringLiteral("<?xml version=\"1.0\" encoding=\"UTF - 8\" "
                             "standalone=\"yes\"?>")
           << "\r\n";
    config << QStringLiteral("<指显工作站信息>") << "\r\n";
    for (auto &s : allStations) {
      config << s.toXmlString() << "\r\n";
    }
    config << QStringLiteral("</指显工作站信息>") << "\r\n";
    file.close();
  }
}

/*!
根据工作站唯一标识(\see stationId)查找工作站信息
@param const std::string & stationId 工作站唯一标识
@return StationInfo* 指定标识的工作站指针,如果不存在,返回NULL
作者：cokkiy（张立民)
创建时间：2015/11/24 11:19:48
*/
StationInfo *StationList::findById(const std::string &stationId) {
  try {
    for (auto &s : allStations) {
      if (s.stationId != NULL && s.stationId.compare(stationId.c_str()) == 0) {
        return &s;
      }
    }
  } catch (...) {
  }
  return NULL;
}

/*!
加入工作站到工作站列表
@param const \see StationInfo& station 要加入的工作站
@return void
作者：cokkiy（张立民)
创建时间：2015/11/04 12:32:52
*/
void StationList::push(const StationInfo &station) {
  this->allStations.push_back(station);
  // currentStations 是allStations的投影,内部存储的是同一个对象
  this->currentStations.push_back(&allStations.back());
  emit stationAdded(&allStations.back());
  emit stationListChanged();
}
