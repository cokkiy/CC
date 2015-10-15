#include "..\..\..\..\include\net\netDepends"
#include "SimpleLogger.h"
#include <time.h>
#include <thread>
#include <fcntl.h>


//构造函数,申请资源
SimpleLogger::SimpleLogger() :file(-1)
{
    //初始化第一个写入缓存区
    //buf = new unsigned char[FlushPacketCount*packetSize];
}

//析构函数,释放资源
 SimpleLogger::~SimpleLogger()
 {
     //stop first
     stop();
     //释放全部缓存区
     for (auto buf:bufList)
     {
         if (buf != NULL)
         {
             delete[] buf;
             buf = NULL;
         }
     }
     delete buf;    
 }


/*!
记录接收到的网络数据
@param const stru_Param & param  接收到的网络数据
@return void
作者：cokkiy（张立民)
创建时间：2015/10/08 21:59:26
*/
// void SimpleLogger::log(/*const stru_Param& param*/)
// {
//     // get current time
//     time_t cur;
//     time(&cur);  
// 
//     //put to buffer
//     //put2buf(cur, param);  
// }

/*!
开始记录数据
@return void
作者：cokkiy（张立民)
创建时间：2015/10/08 21:59:12
*/
void SimpleLogger::start(string path)
{
    time_t cur;
    time(&cur);
    char* filename = new char[path.length() + 32];
    sprintf(filename, "%s/net-received-%d.dat", path.c_str(), cur);
    file = _open(filename, _O_BINARY | _O_CREAT | _O_WRONLY, _S_IWRITE);
    delete filename;
    if (file != -1)
    {
        //writer = new std::thread(&SimpleLogger::write2file, this);
    }

    //save path
    this->path = path;
}

// stop record
void SimpleLogger::stop()
{
    bStop = true;
    if (writer != NULL)
    {
        writer->join();
        delete writer;
        writer = NULL;
    }

//     if (index < bufSize)
//     {
//         //还有数据没有写入
//         if (file != -1)
//         {
//             _write(file, buf, index);            
//         }
//     }
    //_close(file);    //关闭文件

    time_t cur;
    time(&cur);
    char* filename = new char[path.length() + 32];
     //open parsed packet count file
    memset(filename, 0, path.length() + 32);
    sprintf(filename, "%s/packet-count-%d.txt", path.c_str(), cur);
    FILE* file4packetCount = fopen(filename, "w");
    delete filename;

    //关闭计数文件
    if (file4packetCount != NULL)
    {
        //write count to file
        fprintf(file4packetCount, "Received Frame:%d, After Parsed:%d packet.", receivedDataCount, parsedPacketCount);
        fclose(file4packetCount);
        file4packetCount = NULL;
    }    
}

//记录处理后的包个数
void SimpleLogger::logPacketCount()
{
    mutexForParsedCount.lock();
    parsedPacketCount++;
    mutexForParsedCount.unlock();
}

//记录网络接收到的帧个数
void SimpleLogger::logReceivedPacketCount()
{
    mutexForReceivedCount.lock();
    receivedDataCount++;
    mutexForReceivedCount.unlock();
}


//把数据和时间放入到缓冲区
// void SimpleLogger::put2buf(time_t cur, const stru_Param& param)
// {
//     int curIndex = index;
//     mutexForBuf.lock();
//     if (index + packetSize < bufSize)
//     {
//         // move index to next header
//         index += packetSize;
//     }
//     else
//     {
//         unique_lock<mutex> lock(mutexForNotify);
//         //put old buf to buf list
//         bufList.push_back(buf);
//         //notify 
//         hasItemCV.notify_one();
//         // then, create new buf
//         buf = new unsigned char[FlushPacketCount*packetSize];
//         index = 0;
//     }
//     mutexForBuf.unlock();
// 
//     // put to buf
//     memcpy(buf + curIndex, &cur, sizeof(time_t));
//     curIndex += sizeof(time_t);
//     
//     memcpy(buf + curIndex, &param, sizeof(stru_Param));
//     curIndex += sizeof(stru_Param);    
// }
// 
// // write buffered data to file
// void SimpleLogger::write2file()
// {
//     while (!bStop)
//     {
//         unique_lock<mutex> lock(mutexForNotify);
//         while (bufList.empty())
//         {
//             hasItemCV.wait_for(lock, std::chrono::milliseconds(100));
//             if (bStop)
//             {
//                 break;
//             }
//         }        
// 
//         unsigned char* tempBuf = bufList.front();
//         if (_write(file, tempBuf, bufSize) == -1)
//         {
//         }
//         bufList.pop_front();
//         delete[] tempBuf;
//     }
//    
// }
