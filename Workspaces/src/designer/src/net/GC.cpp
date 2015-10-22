#include "GC.h"
#include <thread>
#include <QDebug>
using namespace std::literals;

//get total memory size
extern size_t getMemorySize();
//get current rss (physical memory used by this process)
extern size_t getCurrentRSS();
//get peak rss (physical memory used by this process)
extern size_t getPeakRSS();

#define INDEX(tn,pn) (((unsigned int)tn<<16)|(unsigned short)pn)

/*!
构造函数,创建GC对象
@param  HistoryParamBuffer* pHistoryParamBuffer 指向历史缓存区管理类的指针
@param unsigned short percent 允许本程序最大占用系统内存百分比（30-90)
@return GC对象
作者：cokkiy（张立民)
创建时间：2015/10/16 15:59:57
*/
GC::GC(HistoryParamBuffer* pHistoryParamBuffer)
{
    this->pParamMap = &pHistoryParamBuffer->m_HistoryParamBuf;
}

//释放资源
GC::~GC()
{
    //停止回收
    stop();
}

/*!
启动垃圾回收工具,应该在系统启动的时候启动
@return void
作者：cokkiy（张立民)
创建时间：2015/10/16 15:56:04
*/
void GC::start()
{
    //get total system physical memory 
    total = getMemorySize();
    QString msg = QObject::tr("Total installed memory in your system is : %1 bytes. GC starting...");
    qWarning() << msg.arg(total);

    //估计控制百分比
    estimatePercent();

    std::thread gcThread(&GC::collect, this, pParamMap);
}

/*!
结束垃圾回收,应该在系统退出的时候调用
@return void
作者：cokkiy（张立民)
创建时间：2015/10/16 17:51:43
*/
void GC::stop()
{
    bQuit = true;
}

// periodcally collect history param list item
// and remove unused
void GC::collect(HistoryParamMap* pParamMap)
{
    while (true)
    {
        if (bQuit)
        {
            break;
        }

        //sleep until duration go on
        std::this_thread::sleep_for(sleepDuration);

        //check memory usage
        current = getCurrentRSS();
        if (current == 0L)
        {
            current = getPeakRSS();
        }

        curPercent = ((float)current) / total;
        // adjust sleep time
        adjustSleepTime();

        if (curPercent < percent*0.8 / 100.0)
        {
            // 比设置值小于80%前，不做任何清理
            continue;
        }

        for (auto &param : *pParamMap)
        {
            GCWrapper& wrapper = param.second;
            if (collectIndex == 0)
            {
                //第一轮只清理未使用的参数
//                 if (wrapper.visited == 0)
//                 {
//                     // first class collected
//                     wrapper.noWrite = true;                    
//                     HistoryParams* org = wrapper.pParamsBuf;
//                     wrapper.pParamsBuf = new HistoryParams();
//                     for (auto& item : *org)
//                     {
//                         if (item != nullptr)
//                         {
//                             item->clear();
//                             delete item;
//                             item = nullptr;
//                         }
//                     }
//                     org->clear();
//                     delete org;
//                     wrapper.gcTimes++;
//                 }
            }
            else if (collectIndex == 1)
            {
                size_t count = wrapper.pParamsBuf->size() / 3;
                if (count == 0 && wrapper.pParamsBuf->size() >= 2)
                {
                    count = 1; // 有2个的话,清掉1个
                }
                //第二轮清理每个参数的1/3
                for (size_t i = 0; i < count; i++)
                {
                    //先把需要清理的参数从list中摘出来
                    auto item = wrapper.pParamsBuf->front();
                    wrapper.pParamsBuf->pop_front();
                    willBeCollected.push_back(item);
                }                
            }
            else if(collectIndex==2)
            {
                if (willBeCollected.size() != 0)
                {
                    //执行清理工作
                    size_t count = willBeCollected.size();
                    for (size_t i = 0; i < count; i++)
                    {
                        auto item = willBeCollected.front();
                        willBeCollected.pop_front();
                        if (item != nullptr)
                        {
                            item->clear();
                            delete item;
                        }
                    }
                }
            }
            wrapper.visited = 0;
            wrapper.writed = 0;
        }

        collectIndex++;
        if (collectIndex > 2)
        {
            //重置回收次数
            collectIndex = 0;
        }
    }   
}

// adjust gc sleeping time (or sweeping frequency) based on memory 
// consume speed
void GC::adjustSleepTime()
{
    // 10%
    if ((prevPercent - curPercent) > 0.5)
    {
        sleepDuration = sleepDuration / 10;
    }
    else if ((prevPercent - curPercent) > 0.4)
    {
        sleepDuration = sleepDuration / 8;
    }
    else if ((prevPercent - curPercent) > 0.3)
    {
        sleepDuration = sleepDuration / 5;
    }
    else if ((prevPercent - curPercent) > 0.2)
    {
        sleepDuration = sleepDuration / 4;
    }
    else if ((prevPercent - curPercent) > 0.1)
    {
        sleepDuration = sleepDuration / 2;
    }

    //最小不能小于2s
    sleepDuration = sleepDuration >= 2s ? sleepDuration : 2s;
    
    //最大不能大于1min
    if (prevPercent != 0 && (prevPercent - curPercent) <= 0.05 && sleepDuration < 30s)
    {
        sleepDuration++;
    }

    if (curPercent >= (percent - 20) / 100.0)
    {
        sleepDuration = 5s;
    }

    prevPercent = curPercent;
}


