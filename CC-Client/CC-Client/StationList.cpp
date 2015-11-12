#include "StationList.h"

/*!
获取当前工作站数量
@return int 工作站数量
作者：cokkiy（张立民)
创建时间：2015/11/04 12:06:18
*/
int StationList::count() const
{
    return currentStations.size();
}

/*!
获取\see row位置的工作站
@param int row 指定位置
@return StationInfo \see row位置的工作站信息
作者：cokkiy（张立民)
创建时间：2015/11/04 12:07:09
*/
StationInfo* StationList::at(int row)
{
    return currentStations[row];
}

/*!
获取\see row位置的工作站
@param int row 指定位置
@return StationInfo \see row位置的工作站信息
作者：cokkiy（张立民)
创建时间：2015/11/04 12:07:09
*/
StationInfo* StationList::operator[](int row)
{
    return currentStations[row];
}

/*!
获取\see row位置的工作站引用
@param int row 指定位置
@return StationInfo \see row位置的工作站引用
作者：cokkiy（张立民)
创建时间：2015/11/04 12:07:09
*/
StationInfo& StationList::ref(int row)
{
    return *currentStations[row];
}

/*!
获取\see row位置的工作站(const)
@param int row 指定位置
@return StationInfo \see row位置的工作站信息
作者：cokkiy（张立民)
创建时间：2015/11/10 21:55:38
*/
StationInfo* StationList::at(int row) const
{
    return currentStations[row];
}

/*!
获取指定工作站在列表中的位置
@param StationInfo * pStation 工作站
@return int
作者：cokkiy（张立民)
创建时间：2015/11/12 10:50:06
*/
int StationList::indexOf(const StationInfo* pStation) const
{
    int index = -1;
    bool isExists = false;
    for (auto p : currentStations)
    {
        index++; // first is 0
        if (pStation == p)
        {
            isExists = true;
            break;
        }
    }
    return isExists ? index : -1;
}

/*!
根据@param filterString过滤工作站
@param const FilterOperations& filter 过滤条件
@return void
作者：cokkiy（张立民)
创建时间：2015/11/04 12:29:36
*/
void StationList::filter(const FilterOperations& filter)
{
    this->filterOperations = filter;
}

/*!
根据\param sortby排序工作站列表
@param \see SortBy sortby 排序方式
@return void
作者：cokkiy（张立民)
创建时间：2015/11/04 12:32:52
*/
void StationList::sort(SortBy sortby/*= SortBy::IP*/)
{
    this->sortby = sortby;
    switch (sortby)
    {
    case StationList::IP:
        //根据IP正排序
        break;
    case StationList::IP_DESC:
        //根据IP逆排序
        break;
    case StationList::State:
        //根据状态正排序
        break;
    case StationList::State_DESC:
        //根据状态逆排序
        break;
    case StationList::Name:
        //根据名称正排序
        break;
    case StationList::Name_DESC:
        //根据名称逆排序
        break;
    default:
        //不排序
        break;
    }    
}

//过滤并排序
void StationList::filterANDsort()
{

}

/*!
加入工作站到工作站列表
@param const \see StationInfo& station 要加入的工作站
@return void
作者：cokkiy（张立民)
创建时间：2015/11/04 12:32:52
*/
void StationList::push(const StationInfo& station)
{
    this->allStations.push_back(station);
    //currentStations 是allStations的投影,内部存储的是同一个对象
    this->currentStations.push_back(&allStations.back());
}
