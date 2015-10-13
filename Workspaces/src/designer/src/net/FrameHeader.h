// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：frameheader.h
// 文件摘要： 定义了PDXP和C3I数据帧帧头
//  
// 原始版本：1.0
// 作    者：cokkiy
// 完成日期：2015-09-17
//  
// *************************** 修改记录 ************************** //
// 版    本：
// 修 改 者：
// 完成日期：
// 修改内容：
// 
// *************************************************************** //
// 
#pragma once


#pragma pack(1)
//921 C3I / PDXP帧头中的日期结构
struct HeaderDate
{
    unsigned	Day : 4;//日
    unsigned	Day_10 : 4;//十日
    unsigned	Month : 4;//月
    unsigned	Month_10 : 4;//十月
    unsigned 	Year : 4;//年
    unsigned 	Year_10 : 4;//十年
    unsigned 	Year_100 : 4;//百年
    unsigned 	Year_1000 : 4;//千年	
};

//921 C3I帧头结构
// gcc中long的长度会根据系统（32/64)改变
struct C3IFrameHeader
{
    unsigned char			frame_type;//帧类别1个字节		
    unsigned char				info_type;//信息类别1个字节		
    unsigned short				taskcord;//任务代号2个字节		    
    unsigned short				datalen;//数长2个字节		   
    unsigned char				sourse;//信源1个字节 
    unsigned char				dest;//信宿1个字节		
    unsigned int				framecounter;//帧序号4个字节
    HeaderDate	date;//日期4个字节 
    unsigned int				time;//时间4个字节
    unsigned int			by;//备用4个字节
};

//PDXP帧头结构(32字节)
// gcc中long的长度会根据系统（32/64)改变
struct PDXPFrameHeader
{
    unsigned char		Version;//版本号1字节
    unsigned short		MID;	//任务标志码2字节
    unsigned int		SID;	//信源地址4字节
    unsigned int		DID;	//信宿地址4字节
    unsigned int		BID;	//信息标志码4字节
    unsigned int		FrameNo;//包序号4字节
    unsigned char		Flag;	//信息处理标志1字节
    unsigned int		Temp;	//预留4字节
    unsigned short		Date;	//日期2字节
    unsigned int		Time;	//时间4字节
    unsigned short		DataLength;	//数据域长度2字节
    PDXPFrameHeader()
    {
        Version = 0x80;
        MID = 0;
        SID = 0;
        DID = 0;
        BID = 0;
        FrameNo = 0;
        Flag = 0x00;
        Temp = 0;
    }
};
#pragma pack()
