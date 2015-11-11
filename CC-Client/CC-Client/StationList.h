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
    获取当前工作站数量
    @return int 工作站数量
    作者：cokkiy（张立民)
    创建时间：2015/11/04 12:06:18
    */
    int count() const;

    /*!
    获取\see row位置的工作站(引用)
    @param int row 指定位置
    @return StationInfo \see row位置的工作站信息
    作者：cokkiy（张立民)
    创建时间：2015/11/04 12:07:09
    */
    StationInfo& at(int row); 

    /*!
    获取\see row位置的工作站(const)
    @param int row 指定位置
    @return StationInfo \see row位置的工作站信息
    作者：cokkiy（张立民)
    创建时间：2015/11/10 21:55:38
    */
    StationInfo at(int row) const;

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

