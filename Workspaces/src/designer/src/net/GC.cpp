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

/*!
构造函数,创建GC对象
@param unsigned short percent 允许本程序最大占用系统内存百分比（30-90)
@return GC对象
作者：cokkiy（张立民)
创建时间：2015/10/16 15:59:57
*/
GC::GC(unsigned short percent/*=80, default*/)
{
    if (percent < 30)
        percent = 30;
    if (percent > 90)
        percent = 90;
    this->percent = percent;
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
    qDebug() << msg.arg(total);

    std::thread gcThread(&GC::collect, this);
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
void GC::collect()
{
    while (true)
    {
        if (bQuit)
        {
            break;
        }
        //check memory here
        size_t current = getCurrentRSS();
        if (current == 0L)
        {
            current = getPeakRSS();
        }

        float curPercent = ((float)current) / total;
        if (curPercent > percent - 10)
        {
            // do collect
        }

        std::this_thread::sleep_for(2s);
    }   
}


