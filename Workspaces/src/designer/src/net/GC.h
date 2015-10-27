/*Class for garbage collection.
 * Remove all unused param item from history param list.
 * When GC start it detectes how much physocal memory installed in system,
 * and periodcally checking how many memory used by this process.
 * Author: cokkiy (张立民)
 * 2015-10-16
 * Version:v1.0
 */
#pragma once
#include <chrono>
#include <list>
#include <thread>
#include "HistoryBufferManager.h"
using namespace std::chrono;

/*Class for garbage collection*/
class GC
{
public:
    /*!
    构造函数,创建GC对象
    @param  HistoryParamBuffer* pHistoryParamBuffer 指向历史缓存区管理类的指针
    @param unsigned short percent 允许本程序最大占用系统内存百分比（30-90)
    @return GC对象
    作者：cokkiy（张立民)
    创建时间：2015/10/16 15:59:57
    */
    GC(HistoryBufferManager::HistoryParamMap* pHistoryParamBuffer);
    
    /*Deconstructor*/
    ~GC();

    
    /*!
    启动垃圾回收工具,应该在系统启动的时候启动
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/16 15:56:04
    */
    void start();   

    /*!
    结束垃圾回收,应该在系统退出的时候调用
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/16 17:51:43
    */
    void stop();

private:
    const size_t _4G = 4 * 1024 * 1024 * 1024ULL; // 4G bytes
    const size_t _8G = 8 * 1024 * 1024 * 1024ULL; // 8G bytes
    const size_t _16G = 16 * 1024 * 1024 * 1024ULL; //16G bytes
    //最大允许的占比,允许本程序最大占用系统内存百分比(30-90)
    unsigned short percent;

    //total mem in system
    size_t total;
    // current memory usage
    size_t current;
    // current percent of memory usage
    float curPercent;
    // prev (sample times) percent of memory usage
    float prevPercent;

    //退出程序
    bool bQuit=false;

    //sleep duration, default is 10s
    seconds sleepDuration=(seconds)10;
    
    //history param map
    HistoryBufferManager::HistoryParamMap* pParamMap;

    //实际回收次数
    unsigned int collectIndex = 0;

    //需要回收的Vector列表
    std::list<std::vector<HistoryParam>*> willBeCollected;

    // all params index
    //list<unsigned int> paramsIndex;
   
    // periodcally collect history param list item
    // and remove unused
    void collect(HistoryBufferManager::HistoryParamMap* pParamMap);

    // adjust the gc sleep time
    void adjustSleepTime();

    std::thread* gcThread=nullptr;

    //估计控制百分比
    inline void estimatePercent()
    {
        // get control percent
        if (total <= _4G)
        {
            // 内存小于4G
            percent = 80;
        }
        else if (total <= _8G)
        {
            // 6g-8g
            percent = 85;
        }
        else if (total <= _16G)
        {
            percent = 90;
        }
        else
        {
            percent = 95;
        }
    }
};

