// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：config.cpp
// 文件摘要：配置信息类实现文件，用来实现配置信息的加载。
// 
// 原始版本：1.0
// 当前版本：1.0
// 作    者：肖胜杰
// 完成日期：


#include <qsystemdetection.h>
#include <QDir>
#include <QDebug>
#include "config.h"
#include "selfshare/src/xmlproc/xmlproc.h"

//**********************************************************************
//构造函数 
//
//
//**********************************************************************
Config::Config():BaseObject()
{	
}

//**********************************************************************
//加载配置文件,只加载table_info_global_配置文件
//**********************************************************************
bool Config::LoadConfig(QString dir)
{
    bool result = true;

    QDir qdir(dir);
    QFileInfoList fileList = qdir.entryInfoList();
    QFileInfoList::Iterator thefileIterator;
    QByteArray ba;

    for(thefileIterator = fileList.begin();
        thefileIterator != fileList.end();
        thefileIterator++)
    {
        //取出文件名
        QString fileName = (*thefileIterator).fileName();
        if (fileName.startsWith("table_info_global_", Qt::CaseInsensitive)
            && fileName.endsWith(".xml", Qt::CaseInsensitive))
        {
            QString filePath = QString("%1/%2").arg(dir).arg(fileName);
            //sprintf(tempDir, "%s/%s", dir, fileName.toStdString().c_str());
            if (false == LoadTableInfo(filePath.toStdString().c_str()))
            {
                qWarning() << QObject::tr("Loading table info config error.");
                result = false;
            }
        }
    }

	return result;
}

//**********************************************************************
//加载表信息
//**********************************************************************
bool Config::LoadTableInfo(const char* path)
{
    XmlProc xmlproc;												 //用来处理xml文件
    //unsigned char tempDev, tempThreadNo;		 //用来存储设备号和线程号的临时变量
    ElSave::iterator theIterator;						 //用来遍历xml配置文件的迭代器。

    if(true == xmlproc.LoadSource(path))     //加载xml信息处理线程配置文件
    {
        if(true == xmlproc.XmlParse())				 //解析xml信息处理线程配置文件
        {
            for(theIterator = xmlproc.m_elSave.begin();theIterator != xmlproc.m_elSave.end();theIterator++)
            {
                //本机需加入的组地址
                if(0 == theIterator->m_name.compare("Table"))																		//是线程号标签
                {
                    unsigned short tempInt = 0;
                    while((theIterator != xmlproc.m_elSave.end())&&(0 != theIterator->m_name.compare( "TableIndex"))) theIterator++;
                    if(theIterator == xmlproc.m_elSave.end()) break;

                    if (0 == theIterator->m_name.compare( "TableIndex"))
                    {
                        tempInt = atoi(theIterator->m_val.c_str());
                    }
                    while((theIterator != xmlproc.m_elSave.end())&&(0 != theIterator->m_name.compare( "TableName"))) theIterator++;
                    if(theIterator == xmlproc.m_elSave.end()) break;

                    if (0 == theIterator->m_name.compare( "TableName"))
                    {
                        if(tempInt == 0) return false;
                        m_tableInfo[tempInt] = theIterator->m_val;
                    }

                    if(theIterator == xmlproc.m_elSave.end()) break;
                }
            }
        }
        else 												//xml文件解析错误
        {
            QString str= QObject::tr("Parse table info file:%1  failure, file format error.");
            qWarning() << str.arg(path);
            xmlproc.Unload();
            return false;
        }
    }
    else 													//xml文件载入错误
    {
        QString str=QObject::tr("Parse table info file:%1  failure, file format error.");
        qWarning() << str.arg(path);
        xmlproc.Unload();												  //存储错误代码
        return false;
    }
    xmlproc.Unload();

    return true;
}

//**********************************************************************
//析构函数
//
//
//**********************************************************************
Config::~Config()
{

}



