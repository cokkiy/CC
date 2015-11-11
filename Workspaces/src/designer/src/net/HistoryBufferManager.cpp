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
    GCWrapper& wrapper = m_historyParamBuffer[INDEX(tabNo, paramNo)];

    while (wrapper.clearing)
    {
        //等待直到清除数据结束
        //该处不会死锁是基于清屏不会连续进行这一假设的
    }
    wrapper.reading = true;

    HistoryParams* pbuf = wrapper.getBuffer();
    int t_date, t_time = 0;
    //从最后一个vector开始取数据
    for (it = pbuf->rbegin(); it != pbuf->rend(); it++)
    {
        vector<HistoryParam>* pVector = *it;
        HistoryParam& param = pVector->front();
        size_t count = 0;
        if(wrapper.currentVector==pVector)
        {
            count = wrapper.curVectorIndex;
        }
        else
        {
            count = pVector->size();
        }
        if (param.getDate() > date || (param.getDate() == date&&param.getTime() > time))
        {
            //复制该vector中全部数据到list,retBuf是按时间从前到后排序的
            for(int pos = count-1;pos >= 0;pos--)
            {
                HistoryParam& t_param = pVector->at(pos);
                retBuf.push_front(t_param);
                t_date = t_date>t_param.getDate()?t_date:t_param.getDate();
                t_time = t_time>t_param.getTime()?t_time:t_param.getTime();
            }
        }
        else
        {
            for(int pos = count-1;pos >= 0;pos--)
            {
                HistoryParam& t_param = pVector->at(pos);
                if (t_param.getDate() > date || (t_param.getDate() == date&&t_param.getTime() > time))
                {
                    //把该参数放到retBuf前面,retBuf是按时间从前到后排序的
                    retBuf.push_front(t_param);
                    t_date = t_date>t_param.getDate()?t_date:t_param.getDate();
                    t_time = t_time>t_param.getTime()?t_time:t_param.getTime();
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
    wrapper.reading = false; //读取结束
    return retBuf;
}

/*!
获取指定表号,参数号的index之后的历史数据
@param unsigned short tabNo 表号
@param unsigned short paramNo 参数号
@param size_t index 当前索引
@return std::list<HistoryParam> index之后的历史数据
作者：cokkiy（张立民)
创建时间：2015/10/29 12:20:25
*/
std::list<HistoryParam> HistoryBufferManager::getParams(unsigned short tabNo, unsigned short paramNo, size_t & index)
{
    list<HistoryParam> retBuf; 
    GCWrapper& wrapper = m_historyParamBuffer[INDEX(tabNo, paramNo)];    
    while (wrapper.clearing)
    {
        //等待直到清除数据结束
        //该处不会死锁是基于清屏不会连续进行这一假设的
    }
    wrapper.reading = true;
    //根据GC修正index
    size_t realIndex = index - ((size_t)wrapper.collectedCount)* wrapper.vectorSize;
    if (realIndex < 0)
        realIndex = 0; 

    //计算可以跳过的Vector个数
    size_t omitted_vector_count = realIndex / wrapper.vectorSize;
    //开始位置（跳过omitted_vector_count个vector后的开始位置)
    unsigned short indexInVector = realIndex%wrapper.vectorSize;

    HistoryParams* pbuf = wrapper.getBuffer();
    HistoryParams::iterator iter = pbuf->begin();
    //移动到omitted_vector_count处
    for (int i = 0; i < omitted_vector_count; i++)
    {
        if (iter != pbuf->end())
            iter++;
        else
            return retBuf; //超出范围,返回空
    }
    for (; iter != pbuf->end();iter++)
    {
        //开始取数据
        vector<HistoryParam>* pVector = *iter;
        size_t count = 0;
        if (wrapper.currentVector == pVector)
        {
            count = wrapper.curVectorIndex;
        }
        else
        {
            count = pVector->size();
        }
        for (size_t i = indexInVector; i < count; i++)
        {
            HistoryParam& t_param = pVector->at(i);
            retBuf.push_back(t_param);  //从前向后顺序放
            index++;
        }
        indexInVector = 0; //取完第一个vector的数据后,从头开始取
    }
    wrapper.reading = false; //读取结束
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

/*!
清除beginTabNo和endTabNo之间的全部数据
@param unsigned short beginTabNo 开始表号
@param unsigned short endTabNo 结束表号
@return void
作者：cokkiy（张立民)
创建时间：2015/11/11 15:54:30
*/
void HistoryBufferManager::clear(unsigned short beginTabNo, unsigned short endTabNo)
{
    unsigned int begin = ((unsigned int)beginTabNo) << 16;
    unsigned int end = ((unsigned int)endTabNo) << 16;
    for (auto& paramMapItem : m_historyParamBuffer)
    {
        if (paramMapItem.first >= begin&&paramMapItem.first <= end)
        {
            GCWrapper& wrapper = paramMapItem.second;
            HistoryBufferManager::HistoryParams* org = wrapper.pParamsBuf;
            lockForWriteBuf.lock(); //防止写入异常
            wrapper.pParamsBuf = new HistoryBufferManager::HistoryParams();
            wrapper.curVectorIndex = 0;
            wrapper.collectedCount = 0; //清屏后需要重置读计数
            lockForWriteBuf.unlock();    
            //读可能连续读,但清屏不会是连续的,所以可以安全的置为true后等待读结束
            wrapper.clearing = true; 
            while (wrapper.reading)
            {
                //等待直到读取结束
            }            
            for (auto& item : *org)
            {
                if (item != nullptr)
                {
                    item->clear();
                    delete item;
                    item = nullptr;
                }
            }
            org->clear();
            delete org;
            wrapper.clearing = false;
        }
    }    
}
