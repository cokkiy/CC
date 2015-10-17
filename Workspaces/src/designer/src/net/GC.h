/*Class for garbage collection.
 * Remove all unused param item from history param list.
 * When GC start it detectes how much physocal memory installed in system,
 * and periodcally checking how many memory used by this process.
 * Author: cokkiy (张立民)
 * 2015-10-16
 * Version:v1.0
 */
#pragma once
class GC
{
public:

    /*!
    构造函数,创建GC对象
    @param unsigned short percent 允许本程序最大占用系统内存百分比（30-90)
    @return GC对象
    作者：cokkiy（张立民)
    创建时间：2015/10/16 15:59:57
    */
    GC(unsigned short percent=80);
    virtual ~GC();

    
    /*!
    启动垃圾回收工具,应该在系统启动的时候启动
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/16 15:56:04
    */
    void start();

    /*!
    结束垃圾回收,应该在系统退出的时候调用
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/10/16 17:51:43
    */
    void stop();
private:
    //最大允许的占比,允许本程序最大占用系统内存百分比(30-90)
    unsigned short percent;

    //total mem in system
    size_t total;

    //退出程序
    bool bQuit=false;
    // periodcally collect history param list item
    // and remove unused
    void collect();
};

