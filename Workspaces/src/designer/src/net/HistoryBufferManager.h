﻿#pragma once
#include <list>
#include <vector>
#include <map>
#include <mutex>
#include "selfshare/src/historyparam/historyparam.h"

/************************************************************************
* HistoryBufferManager类定义
* 定义了管理历史数据缓存区的方法和结构
* Ver:1.0
* Author: cokkiy
************************************************************************/
class HistoryBufferManager
{
private:
    // defines HistoryParamStorage : std::list<std::vector<HistoryParam>*>
    typedef std::list<std::vector<HistoryParam>*> HistoryParams;
    
    // History Param wrapper for GC
    class GCWrapper
    {
    private:        
        // visited time since last gc collection
        unsigned int visited = 0;
        // writed count since last gc collection
        unsigned int writed = 0;
        // indicate no write?
        bool noWrite = false;
        //是指针所有者?
        bool isOwner = false;        

        // GC is frient class for GCWrapper
        friend class GC;      

    public:
        //vector size, 20000
        const unsigned short vectorSize = 20000u;
        //垃圾回收数量,指回收了多少个list
        unsigned short collectedCount = 0;
        //当前写入的Vector
        std::vector<HistoryParam>* currentVector;
        //当前Verctor写入计数
        unsigned short curVectorIndex;
        //存储数据的list
        HistoryParams* pParamsBuf = nullptr;
        //正在读取数据
        bool reading = false;
        //正在清除数据
        bool clearing = false;
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
            visited = 0;
            writed = 0;
            noWrite = false;
            collectedCount = 0;
            isOwner = true;
        }


        /*!
        拷贝构造函数,当从list中被拷贝时被调用
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
            collectedCount = ref.collectedCount;
            curVectorIndex = ref.curVectorIndex;
            currentVector = ref.currentVector;
            isOwner = false;
        }

        //不要在析构函数中释放任何资源,除非你知道（很清楚)该函数确切调用时机
        ~GCWrapper()
        {
            //只有所有者才释放指针
            if (isOwner)
                release();
        }


        /*!
        把param存入到历史缓存区中
        @param const HistoryParam & param
        @return void
        作者：cokkiy（张立民)
        创建时间：2015/10/19 9:57:40
        */
        void push(const HistoryParam& param);
       

        /*!
        获取指向历史缓存区的指针
        @return HistoryParams
        作者：cokkiy（张立民)
        创建时间：2015/10/19 10:00:12
        */
        HistoryParams* getBuffer();

        /*!
        释放资源,删除分配的空间,请只在程序关闭前调用一次
        @return void
        作者：cokkiy（张立民)
        创建时间：2015/10/19 10:05:18
        */
        void release();
    };

    //defines HistoryParamMap-->std::map<unsigned int, GCWrapper >
    typedef std::map<unsigned int, GCWrapper > HistoryParamMap;

    //历史数据缓存区
    static HistoryParamMap m_historyParamBuffer;
    // lock for buf write
    static std::mutex lockForWriteBuf;

    // gc instance
    static class GC thisGC;

    // GC is friend of my (HistoryBufferManager)
    friend class GC;
public:
    /*!
    把表号,参数号指定的参数存入历史缓存区
    @param unsigned short tabNo 表号
    @param unsigned short paramNo 参数号
    @param const HistoryParam & param 需要存入的参数
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/24 11:48:55
    */
    static void PushParam(unsigned short tabNo, unsigned short paramNo, const HistoryParam& param);

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
    static std::list<HistoryParam> getParams(unsigned short tabNo, unsigned short paramNo, int & date, int & time);

    /*!
    获取指定表号,参数号的index之后的历史数据
    @param unsigned short tabNo 表号
    @param unsigned short paramNo 参数号
    @param size_t index 当前索引
    @return std::list<HistoryParam>
    作者：cokkiy（张立民)
    创建时间：2015/10/29 12:20:25
    */
    static std::list<HistoryParam> getParams(unsigned short tabNo, unsigned short paramNo, size_t& index);
    /*!
    启动历史缓存区回收管理工作
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/24 12:16:24
    */
    static void startGC();


    /*!
    停止历史缓存区回收管理工作
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/24 12:17:08
    */
    static void stopGC();

    /*!
    清空所有数据并释放所有资源,在程序即将退出的时候调用
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/24 12:28:09
    */
     static void releaseAll();

     /*!
     清除beginTabNo和endTabNo之间的全部数据
     @param unsigned short beginTabNo 开始表号
     @param unsigned short endTabNo 结束表号
     @return void
     作者：cokkiy（张立民)
     创建时间：2015/11/11 15:54:30
     */
     static void clear(unsigned short beginTabNo, unsigned short endTabNo);
};

