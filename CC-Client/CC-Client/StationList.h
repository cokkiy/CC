#pragma once
#include "StationInfo.h"
#include <list>
#include <vector>
#include "FilterOperations.h"
/***************************************************************************
 @class StationList：工作站列表类,定义了对工作站进行排序,过滤等操作
 ***************************************************************************/
class StationList 
{
public:
    //排序方式
    enum SortBy
    {
        IP=0, /*按IP正排序*/
        IP_DESC,/*按IP逆排序*/
        State,/*按状态正排序,正常的在前面*/
        State_DESC,/*按状态逆排序,正常的在后面*/
        Name,/*按名称排序*/
        Name_DESC,/*按名称逆排序*/
    };

    /*!
    复制构造函数(已被删除：禁用)
    @param const StationList & ref 被复制工作站列表
    @return ref的复制对象
    作者：cokkiy（张立民)
    创建时间：2015/11/04 12:22:20
    */
    StationList(const StationList& ref) = delete;

    /*默认构造函数*/
    StationList() = default;

    /*析构函数*/
    ~StationList() = default;

    /*!
    获取当前显示工作站数量(已过滤后的)
    @return int 工作站数量
    作者：cokkiy（张立民)
    创建时间：2015/11/04 12:06:18
    */
    int currentCount() const;

    /*!
    获取全部工作站数量
    @return int 全部工作站数量
    作者：cokkiy（张立民)
    创建时间：2015/11/12 17:13:55
    */
    int total();

    /*!
    获取当前工作站显示列表（过滤后的)中\see row位置的工作站
    @param int row 指定位置
    @return StationInfo \see row位置的工作站信息
    作者：cokkiy（张立民)
    创建时间：2015/11/04 12:07:09
    */
    StationInfo* atCurrent(int row); 
    
    /*!
    获取当前工作站显示列表（过滤后的)\see row位置的工作站引用
    @param int row 指定位置
    @return StationInfo \see row位置的工作站引用
    作者：cokkiy（张立民)
    创建时间：2015/11/04 12:07:09
    */
    StationInfo& refCurrent(int row);

    /*!
    获取当前工作站显示列表（过滤后的)\see row位置的工作站(const)
    @param int row 指定位置
    @return StationInfo \see row位置的工作站信息
    作者：cokkiy（张立民)
    创建时间：2015/11/10 21:55:38
    */
    StationInfo* atCurrent(int row) const;


    /*!
    获取指定工作站在当前显示列表中的位置
    @param StationInfo * pStation 工作站
    @return int
    作者：cokkiy（张立民)
    创建时间：2015/11/12 10:50:06
    */
    int indexAtCurrent(const StationInfo* pStation) const;

    /*!
    查找指定IP的工作站信息
    @param QString ip 工作站IP
    @return StationInfo* 指定IP的工作站指针,如果不存在,返回NULL
    作者：cokkiy（张立民)
    创建时间：2015/11/12 17:41:43
    */
    StationInfo* find(QString ip);

    /*!
    根据工作站唯一标识(\see stationId)查找工作站信息
    @param const std::string & stationId 工作站唯一标识
    @return StationInfo* 指定标识的工作站指针,如果不存在,返回NULL
    作者：cokkiy（张立民)
    创建时间：2015/11/24 11:19:48
    */
    StationInfo* findById(const std::string& stationId);


    /*!
    查找指定mac->IP对应的工作站信息
    @param std::pair<const std::string, std::list<std::string>> 工作站的mac->list<ip>
    @return StationInfo* 指定信息的工作站指针,如果不存在,返回NULL
    作者：cokkiy（张立民)
    创建时间：2015/11/24 10:22:10
    */
    StationInfo* find(const std::pair<const std::string, std::list<std::string>> &);

    /*!
    获取全部工作站列表的开始位置
    @return std::list<StationInfo>::iterator 全部工作站列表的开始位置
    作者：cokkiy（张立民)
    创建时间：2015/11/25 12:31:08
    */
    std::list<StationInfo>::iterator begin()
    {
        return allStations.begin();
    }

    /*!
    获取全部工作站列表的结束位置
    @return std::list<StationInfo>::iterator 全部工作站列表的结束位置
    作者：cokkiy（张立民)
    创建时间：2015/11/25 12:31:45
    */
    std::list<StationInfo>::iterator end()
    {
        return allStations.end();
    }

    /*!
    订阅全部工作站属性变化通知事件
    @param const QObject * receiver 事件接收函数所在对象的指针
    @param const char * member 事件接收函数名称
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/12 17:44:28
    */
    void subscribeAllStationsPropertyChangedEvent(const QObject* receiver, const char* member);
  
    /*!
    订阅全部工作站状态变化通知事件
    @param const QObject * receiver 事件接收函数所在对象的指针
    @param const char * member 事件接收函数名称
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/13 10:57:28
    */
    void subscribeAllStationsStateChangedEvent(const QObject* receiver, const char* member);

    /*!
    根据@param filterString过滤工作站
    @param const FilterOperations&  过滤条件
    @return void 
    作者：cokkiy（张立民)
    创建时间：2015/11/04 12:29:36
    */
    void filter(const FilterOperations& filter);

    /*!
    根据\param sortby排序工作站列表
    @param \see SortBy sortby 排序方式
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/04 12:32:52
    */
    void sort(SortBy sortby = SortBy::IP);

    /*!
    加入工作站到工作站列表
    @param const \see StationInfo& station 要加入的工作站
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/04 12:32:52
    */
    void push(const StationInfo& station);
private:
    //全部工作站列表
    std::list<StationInfo> allStations;
    //当前（排序+过滤后的)工作站列表
    std::vector<StationInfo*> currentStations;
    //过滤字符串
    FilterOperations filterOperations;
    //排序方式
    SortBy sortby;

    //过滤并排序
    void filterANDsort();    
};

