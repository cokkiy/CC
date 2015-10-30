#include "SimpleLogger.h"
#include <qsystemdetection.h>
#include <time.h>
#include <thread>
#include <fcntl.h>

#ifdef Q_OS_WIN
#include <io.h>
#define S_IROTH _S_IREAD  
#endif

#ifdef Q_OS_LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define _write write
#define _close close
#define _open open
#define _O_BINARY O_TRUNC
#define _O_CREAT O_CREAT
#define _O_WRONLY O_WRONLY
#define _S_IWRITE S_IWRITE
#endif


//构造函数,申请资源
SimpleLogger::SimpleLogger() :file(-1)
{
    //初始化第一个写入缓存区
    buf = new char[bufSize];
}

//析构函数,释放资源
SimpleLogger::~SimpleLogger()
{
    //stop first
    stop();

    if (buf != nullptr)
    {
        delete[] buf;
        buf = nullptr;
    }

}


/*!
记录接收到的网络数据
@param char* buf  接收到的网络原始数据
@param unsigned int size  接收到的网络原始数据大小
@return void
作者：cokkiy（张立民)
创建时间：2015/10/08 21:59:26
*/
void SimpleLogger::log(char* buf, unsigned int size)
{
    //put to buffer
    put2buf(buf, size);
}

/*!
开始记录数据
@return void
作者：cokkiy（张立民)
创建时间：2015/10/08 21:59:12
*/
void SimpleLogger::start()
{
    if (file != -1)
    {
        writer = new std::thread(&SimpleLogger::write2file, this);
    }
}

// stop record
void SimpleLogger::stop()
{
    bStop = true;
    if (writer != nullptr)
    {
        writer->join();
        delete writer;
        writer = nullptr;
    }

    if (index < bufSize)
    {
        //还有数据没有写入
        if (file != -1)
        {
            _write(file, buf, index);
        }
    }
    if (file != -1)
        _close(file);    //关闭文件

//    time_t cur;
//    time(&cur);
//    char* filename = new char[path.length() + 32];
//     //open parsed packet count file
//    memset(filename, 0, path.length() + 32);
//    sprintf(filename, "%s/packet-count-%d.txt", path.c_str(), cur);
//    FILE* file4packetCount = fopen(filename, "w");
//    delete filename;

//    //关闭计数文件
//    if (file4packetCount != NULL)
//    {
//        //write count to file
//        fprintf(file4packetCount, "Received Frame:%d, After Parsed:%d packet.", receivedDataCount, parsedPacketCount);
//        fclose(file4packetCount);
//        file4packetCount = NULL;
//    }
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

/*!
初始化
@param string path 记录文件存放路径
@param bool isPrimaryChannel  是主用通道数据吗?默认是
@return bool 初始化是否成功
作者：cokkiy（张立民)
创建时间：2015/10/28 9:59:28
*/
bool SimpleLogger::init(string path, bool isPrimaryChannel)
{
    _tzset();
    time_t cur;
    time(&cur);
    struct tm* now = localtime(&cur);    
    char filename[2048];
    if (isPrimaryChannel)
    {
        sprintf(filename, "%s/received-%d%02d%02d-%02d%02d%02d-primary-channel.dat\0", path.c_str(),
            now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    }
    else
    {
        sprintf(filename, "%s/received-%d%02d%02d-%02d%02d%02d-backup-channel.dat\0", path.c_str(),
            now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    }
    file = _open(filename, _O_BINARY | _O_CREAT | _O_WRONLY, _S_IWRITE | S_IROTH);

    //save path
    this->path = path;

    return file != -1;
}

//把数据和时间放入到缓冲区
void SimpleLogger::put2buf(char* src, unsigned int size)
{
    int curIndex = index;
    mutexForBuf.lock();
    if (index + size < bufSize)
    {
        // move index to next header
        index += size;
    }
    else
    {
        unique_lock<mutex> lock(mutexForNotify);
        //put old buf to buf list
        bufList.push_back(Buf{ buf,index });
        //notify 
        hasItemCV.notify_one();
        //create new buf
        buf = new char[bufSize];
        index = size; // index+=size;
        curIndex = 0;
    }
    mutexForBuf.unlock();

    // put to buf    
    memcpy(buf + curIndex, src, size);
}

// write buffered data to file
void SimpleLogger::write2file()
{
    bool quit = false;
    while (!quit)
    {
        unique_lock<mutex> lock(mutexForNotify);
        while (bufList.empty())
        {
            hasItemCV.wait_for(lock, std::chrono::milliseconds(100));
            if (bStop)
            {
                //退出
                quit = true;
                break;
            }
        }

        if (!bufList.empty())
        {
            Buf & tempBuf = bufList.front();
            if (_write(file, tempBuf.pBuf, tempBuf.size) == -1)
            {
            }
            delete[] tempBuf.pBuf;
            tempBuf.pBuf = nullptr;
            bufList.pop_front();
        }
    }

}
