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

    //记录接收到的数据包
    //void log(const stru_Param& param);

    //记录分析处理后的数据包个数
    void logPacketCount();

    //记录接收到的数据包个数
    void logReceivedPacketCount();

    //开始记录数据
    void start(string path);

    void stop();
    
private:
    //一次写入多少个包
    const int FlushPacketCount = 13796;
    // size of one packet write to buf
    const int packetSize = 16; // double + uint*2
    //buf size
    const int bufSize = FlushPacketCount*packetSize;

    //写入缓冲区列表
    list<unsigned char*> bufList;

    //当前写入缓存区指针
    unsigned char* buf;

    // current write index of current buf
    int index;

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
    std::thread * writer;

    //把数据和时间放入到缓冲区
    void put2buf(const HistoryParam& param);

    //write bufed data to file
    void write2file();
};

