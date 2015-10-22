#pragma once
#include <vector>
#include "selfshare/src/historyparam/historyparam.h"

// defines HistoryParamStorage : std::list<std::vector<HistoryParam>*>
typedef std::list<std::vector<HistoryParam>*> HistoryParams;
// History Param wrapper for GC
class GCWrapper
{
private:
    //存储数据的list
    HistoryParams* pParamsBuf = nullptr;
    // visited time since last gc collection
    unsigned int visited = 0;
    // writed count since last gc collection
    unsigned int writed = 0;    
    // indicate no write?
    bool noWrite = false;

    //垃圾回收次数
    unsigned short  gcTimes = 0;

    // GC is frient class for GCWrapper
    friend class GC;
    //vector size, 20000
    const unsigned short vectorSize= 20000u;
    //当前写入的Vector
    std::vector<HistoryParam>* currentVector;
    //当前Verctor写入计数
    unsigned short curVectorIndex;
public:

    /*!
    默认构造函数,创建GCWrapper实例
    @return 
    作者：cokkiy（张立民)
    创建时间：2015/10/19 9:49:42
    */
    GCWrapper()
    {
        pParamsBuf = new HistoryParams();        
        curVectorIndex = 0;
        currentVector = nullptr;
    }


    /*!
    拷贝构造函数,当从list中被拷贝是被调用
    @param const GCWrapper & ref 被拷贝对象
    @return 
    作者：cokkiy（张立民)
    创建时间：2015/10/19 9:50:45
    */
    GCWrapper(const GCWrapper& ref)
    {
        this->pParamsBuf = ref.pParamsBuf;
        visited = ref.visited;
        writed = ref.writed;
        noWrite = ref.noWrite;
        gcTimes = ref.gcTimes;
        curVectorIndex = ref.curVectorIndex;
        currentVector = ref.currentVector;
    }

    //不要在析构函数中释放任何资源,除非你知道（很清楚)该函数确切调用时机
    ~GCWrapper()
    {
        //重要的事再说一遍：不要在析构函数中释放任何资源,除非你知道（很清楚)该函数确切调用时机
        //释放资源请调用release（)函数
    }


    /*!
    把param存入到历史缓存区中
    @param const HistoryParam & param
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/19 9:57:40
    */
    void push(const HistoryParam& param)
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
    获取指向历史缓存区的指针
    @return HistoryParams
    作者：cokkiy（张立民)
    创建时间：2015/10/19 10:00:12
    */
    HistoryParams* getBuffer()
    {
        visited++;
        noWrite = false;
        return pParamsBuf;
    }

    /*!
    释放资源,删除分配的空间,请只在程序关闭前调用一次
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/19 10:05:18
    */
    void release()
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
};

