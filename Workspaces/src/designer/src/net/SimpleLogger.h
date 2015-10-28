//简单的记录网络接收到的数据
#pragma once
#include "C3I/stru.h"
#include <list>
#include <mutex>
#include <condition_variable>
#include <io.h>
#include <thread>
#include "selfshare/src/historyparam/historyparam.h"

class SimpleLogger
{
public:
    SimpleLogger();
    ~SimpleLogger();

    /*!
    记录接收到的网络数据
    @param char* buf  接收到的网络原始数据
    @param unsigned int size  接收到的网络原始数据大小
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/08 21:59:26
    */
    void log(char* buf, unsigned int size);

    //记录分析处理后的数据包个数
    void logPacketCount();

    //记录接收到的数据包个数
    void logReceivedPacketCount();


    /*!
    初始化
    @param string path 记录文件存放路径
    @return bool 初始化是否成功
    作者：cokkiy（张立民)
    创建时间：2015/10/28 9:59:28
    */
    bool init(string path);

    /*!
    开始记录数据
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/08 21:59:12
    */
    void start();


    /*!
    停止记录数据
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/28 9:58:48
    */
    void stop();

private:
    //Buf结构,指向缓冲区的指针和大小
    struct Buf
    {
        char* pBuf = nullptr;
        unsigned int size = 0;

        Buf(char* pBuf, unsigned int size)
        {
            this->pBuf = pBuf;
            this->size = size;
        }

        Buf(const Buf& ref)
        {
            this->pBuf = ref.pBuf;
            this->size = ref.size;
        }
    };

    //buf size
    const unsigned int bufSize = 1024*1024U; //1024K Bytes

    //写入缓冲区列表
    list<Buf> bufList;

    //当前写入缓存区指针
    char* buf = nullptr;

    // current write index of current buf
    unsigned int index;

    //mutex for sync buf rw
    mutex mutexForBuf;
    // mutex for notify between put to/get from buf list 
    mutex mutexForNotify;

    // mutex for received count 
    mutex mutexForReceivedCount;
    // mutex for parsed count
    mutex mutexForParsedCount;

    // has item condition
    condition_variable hasItemCV;

    //is stop?
    bool bStop = false;

    //file to record data
    int file;

    //接收数据计数
    unsigned int receivedDataCount = 0;
    //分析包计数
    unsigned int parsedPacketCount = 0;

    //file path 
    string path;
    // thread for writing data to file
    std::thread * writer = nullptr;

    //把数据放入到缓冲区
    void put2buf(char* buf, unsigned int size);

    //write bufed data to file
    void write2file();
};

