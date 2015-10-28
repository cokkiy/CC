#include "HistoryBufferManager.h"
#include "GC.h"
using namespace std;

#define INDEX(tn,pn) (((unsigned int)tn<<16)|(unsigned short)pn)

//init
HistoryBufferManager::HistoryParamMap HistoryBufferManager::m_historyParamBuffer;
std::mutex HistoryBufferManager::lockForWriteBuf;
GC HistoryBufferManager::thisGC(&HistoryBufferManager::m_historyParamBuffer);

/*!
把param存入到历史缓存区中
@param const HistoryParam & param
@return void
作者：cokkiy（张立民)
创建时间：2015/10/19 9:57:40
*/
inline void HistoryBufferManager::GCWrapper::push(const HistoryParam& param)
{
    if (!noWrite)
    {
        if (curVectorIndex == 0 || vectorSize == curVectorIndex)
        {
            std::vector<HistoryParam>* paramsVector = new std::vector<HistoryParam>(vectorSize);
            pParamsBuf->push_back(paramsVector);
            currentVector = paramsVector;
            curVectorIndex = 0;
        }
        currentVector->at(curVectorIndex) = param;
        curVectorIndex++;
    }
}

/*!
释放资源,删除分配的空间,请只在程序关闭前调用一次
@return void
作者：cokkiy（张立民)
创建时间：2015/10/19 10:05:18
*/
void HistoryBufferManager::GCWrapper::release()
{
    if (pParamsBuf != nullptr)
    {
        for (auto& vector : *pParamsBuf)
        {
            if (vector != nullptr)
            {
                vector->clear();
                delete vector;
                vector = nullptr;
            }
        }
        delete pParamsBuf;
        pParamsBuf = nullptr;
    }
}
/*!
获取指向历史缓存区的指针
@return HistoryParams
作者：cokkiy（张立民)
创建时间：2015/10/19 10:00:12
*/
inline HistoryBufferManager::HistoryParams* HistoryBufferManager::GCWrapper::getBuffer()
{
    visited++;
    noWrite = false;
    return pParamsBuf;
}

/*!
把表号,参数号指定的参数存入历史缓存区
@param unsigned short tabNo 表号
@param unsigned short paramNo 参数号
@param const HistoryParam & param 需要存入的参数
@return void 
作者：cokkiy（张立民)
创建时间：2015/10/24 11:48:55
*/
void HistoryBufferManager::PushParam(unsigned short tabNo, unsigned short paramNo, const HistoryParam& param)
{
    auto& wrapper = m_historyParamBuffer[INDEX(tabNo, paramNo)];
    lockForWriteBuf.lock();
    wrapper.push(param);
    lockForWriteBuf.unlock();
}

/*!
获取指定表号,参数号的date.time之后历史数据
@param unsigned short tabNo 表号
@param unsigned short paramNo 参数号
@param int & date 日期
@param int & time 时间
@return std::list<HistoryParam> date:time时间之后的（tabNo,paramNo)指定的参数列表
作者：cokkiy（张立民)
创建时间：2015/10/24 12:00:45
*/
std::list<HistoryParam> HistoryBufferManager::getParams(unsigned short tabNo, unsigned short paramNo, int&  date, int & time)
{
    list<HistoryParam> retBuf;
    HistoryParams::reverse_iterator it;
    auto& wrapper = m_historyParamBuffer[INDEX(tabNo, paramNo)];
    HistoryParams* pbuf = wrapper.getBuffer();
    int t_date, t_time = 0;
    //从最后一个vector开始取数据
    for (it = pbuf->rbegin(); it != pbuf->rend(); it++)
    {
        vector<HistoryParam>* pVector = *it;
        HistoryParam& param = pVector->front();
        size_t count = pVector->size();
        if (param.getDate() > date || (param.getDate() == date&&param.getTime() > time))
        {
            //复制该vector中全部数据到list,retBuf是按时间从前到后排序的
            for (auto iter = pVector->begin(); iter < pVector->begin() + count; iter++)
            {
                retBuf.push_back(*iter);
            }
            t_date = param.getDate();
            t_time = param.getTime();
        }
        else
        {
            //找到vector中的某个元素,使其时间小于等于date.time
            for (auto iter = pVector->end() - 1; iter == pVector->begin(); iter--)
            {
                if (iter->getDate() > date || (iter->getDate() == date&&iter->getTime() > time))
                {
                    //把该参数放到retBuf前面,retBuf是按时间从前到后排序的
                    retBuf.push_front(*iter);
                    t_date = iter->getDate();
                    t_time = iter->getTime();
                }
                else
                {
                    break;
                }
            }
        }
    }
    if (t_time != 0)
    {
        date = t_date;
        time = t_time;
    }
    retBuf.reverse();
    return retBuf;
}

/*!
启动历史缓存区回收管理工作
@return void
作者：cokkiy（张立民)
创建时间：2015/10/24 12:16:24
*/
void HistoryBufferManager::startGC()
{
    thisGC.start();
}

/*!
停止历史缓存区回收管理工作
@return void
作者：cokkiy（张立民)
创建时间：2015/10/24 12:17:08
*/
void HistoryBufferManager::stopGC()
{
    thisGC.stop();
}


/*!
清空所有数据并释放所有资源
@return void
作者：cokkiy（张立民)
创建时间：2015/10/24 12:28:09
*/
void HistoryBufferManager::releaseAll()
{
    //释放历史缓冲区空间
    for (auto it = m_historyParamBuffer.begin(); it != m_historyParamBuffer.end(); it++)
    {
        GCWrapper& wrapper = it->second;
        wrapper.release();
    }
    //清空缓冲区队列
    m_historyParamBuffer.clear();
}
