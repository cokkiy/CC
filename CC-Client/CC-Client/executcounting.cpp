#include "executcounting.h"
using namespace std;

ExecutCounting::ExecutCounting(QObject *parent)
    : QThread(parent)
{

}

ExecutCounting::~ExecutCounting()
{

}

//执行计数工作
void ExecutCounting::run()
{
    bool quit = false;
    while (!quit)
    {
//         unique_lock<mutex> lock(cvMutex);
//         while (inExecutingStateStations.empty())
//         {
//             cv.wait_for(lock, std::chrono::milliseconds(500));
//             if (stop)
//             {
//                 quit = true;
//                 break;
//             }
//         }

        if (!inExecutingStateStations.empty())
        {
            for (auto pStation : inExecutingStateStations)
            {
                if (pStation != NULL)
                {
                    if (pStation->timeout()) //检查是否超时
                    {
                        pStation->setExecuteCounting(0);
                        //超时,则设置超时状态
                        switch (pStation->state().getOperatingStatus())
                        {
                        case StationInfo::Powering:
                            pStation->setState(StationInfo::PowerOnFailure);
                            break;
                        case  StationInfo::AppStarting:
                            pStation->setState(StationInfo::AppStartFailure, QStringLiteral("超时没响应,请等待30秒后再试."));
                            break;
                        case  StationInfo::Rebooting:
                            pStation->setState(StationInfo::RebootFailure);
                            break;
                        case  StationInfo::Shutdowning:
                            pStation->setState(StationInfo::ShutdownFailure);
                            break;
                        default:
                            break;
                        }
                    }
                    else
                    {
                        pStation->setExecuteCounting(pStation->ExecuteCounting() + 1);
                    }
                }
            }
        }
        //等待1秒
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

/*!
添加执行状态工作站
@param StationInfo * 执行状态工作站
@return void
作者：cokkiy（张立民)
创建时间：2015/12/14 17:54:41
*/
void ExecutCounting::addExecutingStateStation(StationInfo* pStation)
{    
    inExecutingStateStations.push_back(pStation);
    inExecutingStateStations.unique();
}

/*!
从执行状态列表移除工作站
@param StationInfo * 要移除的工作站
@return void
作者：cokkiy（张立民)
创建时间：2015/12/14 18:00:03
*/
void ExecutCounting::removeStationFromExecutingStationsList(StationInfo* pStation)
{
    inExecutingStateStations.remove(pStation);
}


/*!
停止执行,退出线程函数
@return void
作者：cokkiy（张立民)
创建时间：2015/12/14 21:24:11
*/
void ExecutCounting::Stop()
{
    this->stop = true;
}
