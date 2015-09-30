// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：const.h
// 文件摘要：综合应用分系统常量定义头文件，用来声明各种常量。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

# define DEBUG 1

#ifndef CONST_H
#define CONST_H

const unsigned short USR_NOT_ENOUGH_MEMORY = 1000;
const unsigned short USR_BUFFER_READ_ERROR = 1001;
const unsigned short USR_NOT_INITIAL_TWICE = 1002;
const unsigned short USR_OVERWRITE_ERROR    = 1003;							 //覆盖错误
const unsigned short USR_WRITE_BUFFER_ERROR = 1004;
const unsigned short USR_WRITE_NULL_POINTER = 1005;             //向空指针写数据
const unsigned short USR_WRITE_OVERFLOW = 1006;             //写内存溢出

const unsigned short USR_FILE_READ_ERROR   = 1010;
const unsigned short USR_FILE_READ_END     = 1011;
const unsigned short USR_FILE_WRITE_ERROR  = 1012;
const unsigned short USR_FILE_OPEN_ERROR   = 1013;
const unsigned short USR_FILEPATH_IS_EMPTY = 1014;         //文件路径为空
const unsigned short USR_RECORD_DATA_ERROR = 1016;

const unsigned short USR_DATALEN_ERROR = 1020;							 //数据长度错误
const unsigned short USR_WRITEBUFFER_OVERFLOW_ERROR = 1021;   //携往缓冲区的数据超过了缓冲区的长度
const unsigned short USR_READ_PIPEDATA_FORMAT_ERROR = 1022;					//读取管道数据格式错误

const unsigned short USR_XMLCONFIGFILE_CONTENT_ERROR = 1030;		 //xml文件内容错误
const unsigned short USR_XMLCONFIGFILE_PARSE_ERROR = 1031;			 //xml文件解析错误
const unsigned short USR_XMLCONFIGFILE_LOAD_ERROR = 1032;        //xml文件加载错误

const unsigned short USR_INFOTABLE_TABLENO_FIND_ERROR = 1040;    //信息约定表中找不到提交的表号
const unsigned short USR_INFOTABLE_PARAMNO_FIND_ERROR = 1041;		 //信息约定表中找不到提交的参数号
const unsigned short USR_PARAMTYPE_NOT_MATCH = 1042;             //参数类型不匹配
const unsigned short USR_NOCODE_MATCH_FREQ = 1043;				//没有与输出频率对应的节点
const unsigned short USR_DECISIONAID_PARAM_VALUE_LEN_ERROR = 1044; //接收到的辅助决策结果参数值长度错误
const unsigned short USR_DECISIONAID_PARAM_TIME_ERROR = 1045; //接收到的辅助决策结果参数采集时间错误
const unsigned short USR_CHILDFRAME_FORMAT_ERROR = 1046;		//子帧格式错（不属于上述的错误）

const unsigned short USR_FLAG_NOT_MATCH = 1050;                  //标志取值错误
const unsigned short USR_FRAME_DES_ERROR = 1051;                  //接收信息帧信宿错误
const unsigned short USR_FRAME_BID_HEAD_ERROR = 1052;                  //接收信息帧信宿错误

const unsigned short USR_MAKE_CHILDFRAME_ERROR = 1060;                  //组子帧错误
const unsigned short USR_FRAMEHEAD_ERROR = 1061;                 //帧头信息错
const unsigned short USR_CHILDFRAMEHEAD_ERROR = 1062;						 //子帧帧头信息错
const unsigned short USR_FRAMETAIL_ERROR = 1063;                 //帧尾码错
const unsigned short USR_CHILDFRAMETAIL_ERROR = 1064;						 //子帧帧尾码错
const unsigned short USR_FRAMETYPE_ERROR = 1065;								 //帧类别错
const unsigned short USR_GET_FRAMEHEAD_INFO_ERROR = 1066;     //获取帧头信息错误
const unsigned short USR_TABLE_EMPTY = 1067;                  //待组帧表中无参数
const unsigned short USR_STRING_OR_CODE_ERROR = 1068;                  //字符串或源码参数长度错误
const unsigned short USR_MAKE_FRAMEHEAD_ERROR = 1069;                  //组帧头出错
const unsigned short USR_PARAMETER_LEN_ERROR = 1070;                   //参数长度错误
const unsigned short USR_AID_FRAMETAIL_ERROR = 1071;                   //辅助决策帧尾码错误
const unsigned short USR_CHILDFRAME_TIME_ERROR = 1072;                   //子帧时间错
const unsigned short USR_CHILDFRAME_COMTYPE_ERROR = 1073;                   //子帧指令类型错


const unsigned short USR_TCP_SOCKET_INIT_ERROR = 1080;                   //TCP套接字初始化失败
const unsigned short USR_UDP_SOCKET_INIT_ERROR = 1081;                   //UDP套接字初始化失败
const unsigned short USR_TCP_SOCKET_BIND_ERROR = 1082;									 //TCP套接字绑定错误
const unsigned short USR_UDP_SOCKET_BIND_ERROR = 1083;									 //UDP套接字绑定错误
const unsigned short USR_SEND_TCP_MESSAGE_ERROR = 1084;                      //发送TCP网络信息失败
const unsigned short USR_SEND_UDP_MESSAGE_ERROR = 1085;                      //发送UDP网络信息失败
const unsigned short USR_TCP_SOCKET_NOT_INIT = 1086;                   //TCP套接字初始化失败
const unsigned short USR_UDP_SOCKET_NOT_INIT = 1087;                   //UDP套接字初始化失败
const unsigned short USR_RECV_TCP_MESSAGE_ERROR = 1088;								 //接收TCP网络信息失败
const unsigned short USR_RECV_UDP_MESSAGE_ERROR = 1089;								 //接收UDP网络信息失败
const unsigned short USR_UDP_DES_ADDR_EMPTY = 1090;										//目标地址为空

const unsigned short USR_SHARE_MEM_CREAT_ERROR = 1100;										//共享内存创建失败
const unsigned short USR_SHARE_MEM_CONNECT_ERROR = 1101;										//共享内存创建失败
const unsigned short USR_SHARE_MEM_SEPE_ERROR = 1102;										//共享内存分离失败
const unsigned short USR_SHARE_MEM_DEL_ERROR = 1103;										//共享内存删除失败

//ITEM类错误代码
const unsigned short USR_CLASS_ITEM_FUN_ADD_EXEC_ERROR = 2000;		//Item类Add方法执行错误

//进程ID
const int MAIN_PROCESS = 0;
const int INFOPROC_PROCESS = 1;
const int NET_PROCESS = 2;

//进程状态
const int RUN = 66;									//运行状态
const int NORMAL = 55;								//未运行状态
const int NORMAL_MAIN = 56;							//未运行状态
const int NORMAL_SUB = 57;							//未运行状态
const int EMERGNCY = 77;							//需要应急启动
const int EMERGNCY_MAIN = 78;						//需要应急启动 主机
const int EMERGNCY_SUB = 79;						//需要应急启动 副机

//系统加锁状态
const char LOCK_SYSTEM = 57;
const char UNLOCK_SYSTEM = 75;

//记住密码标志
const char REMEMBER_PWD = 63;
const char UNREMEMBER_PWD = 36;

//密码数据有效标志
//const char PWD_DATA_VALID = 48;
//const char PWD_DATA_INVALID = 84;

//密码对话框状态
const char INPUTOASSWORD = 0;
const char MANAGEPASSWORD = 1;
const char NOPASSWORD = 2;

//日志类型
const unsigned short LOG_HINT_INFO = 1;
const unsigned short LOG_ERROR_INFO = 2;

//管道数据类型
const unsigned short COMMAND_INFO = 1;
const unsigned short NET_STATUS_INFO = 2;
const unsigned short AID_STATUS_INFO = 4;
const unsigned short LOG_INFO = 6;
const unsigned short TIME_INFO = 8;
const unsigned short FRAME_INFO = 11;
const unsigned short AID_PARAM_INFO = 13;
const unsigned short GROUPFRAME_TIMER_SET = 20;           //组帧（非指挥命令帧）定时器


//信息约定表数据类型
const unsigned short BYTE_TYPE   = 101;										//1字节有符号整数
const unsigned short BIT8_TYPE   = 102;										//1字节无符号整数		
const unsigned short WORD_TYPE  = 103;										//2字节有符号短整
const unsigned short BIT16_TYPE   = 104;									//2字节无符号短整
const unsigned short DWORD_TYPE   = 105;									//4字节有符号长整
const unsigned short BIT32_TYPE  = 106;										//4字节无符号长整(包括日期和时间量)
const unsigned short FLOAT_TYPE  = 107;										//4字节浮点数
const unsigned short DOUBLE_TYPE = 108;										//8字节双精度浮点数
const unsigned short STRING_TYPE = 109;										//字符串类型
const unsigned short CODE_TYPE = 110;										//源码类型
const unsigned short NONE_TYPE   = 111;										//无类型

//帧类别
const unsigned char FULL_FRAME   		= 0;             	  //完整信息帧
const unsigned char SELECT_FRAME   		= 1;             	  //挑点信息帧
const unsigned char COM_FRAME   		= 4;             	  //指挥信息帧
const unsigned char ESC_FRAME   		= 5;             	  //逃逸信息帧

const unsigned char SINGLE_TABLE_SELECT_FRAME   		= 1;             	  //单表挑点信息帧
const unsigned char MULRIPLE_TABLE_SELECT_FRAME   		= 2;             	  //多表挑点信息帧
const unsigned char MULRIPLE_FULL_FRAME   				= 3;             	  //多完整数据段信息帧
const unsigned char MULRIPLE_SINGLE_TABLE_SELECT_FRAME  = 4;             	  //多单表挑点信息帧

//对帧来源分类
const unsigned char CMDDISP_INFO   	= 0;     	           //指显
const unsigned char ASTRONAUT_INFO  	= 1;      	       //宇航员
const unsigned char METEOROLOGY_INFO	= 2;        	     //气象
const unsigned char SPACESHIP_INFO		= 3;          	   //飞船系统
const unsigned char ROCKET_INFO  		= 4;            	   //火箭系统
const unsigned char GRDSERVICE_INFO	= 5;              	 //地勤
const unsigned char ESCAPE_INFO  		= 6;              	 //逃逸
const unsigned char DFCENTER_INFO		= 7;                 //东风中心

//时间格式类别
const unsigned char NOW_TIME   			= 1;             	  //服务器当前时间(从1970年到现在的秒数和微秒数)
const unsigned char BCODE_TIME   		= 5;             	  //B码
const unsigned char FRAME_TIME	   	= 9;	          	  //帧中使用的时间
const unsigned char PRINT_TIME      = 8;                //打印报表使用的时间

//常量定义
const unsigned short FRAME_HEAD_LEN			 = 32;				//帧头长度
const unsigned short MAX_PARAMETER_VAL_LEN   = 4000;			//参数值最大长度（主要指字符串和原码类型）
const unsigned short MAX_PARAMETER_LEN		 = 4003;			//参数最大长度
//const unsigned short MAX_CHILDFRAME_LEN		 = 4062;			//子帧最大长度
const unsigned short MAX_FRAME_LEN   		 = 16384;			//帧最大长度
const unsigned int MAX_BUFERR_NUM            = 50000;			//缓冲区最大限度，当超过时，开始丢弃旧的缓冲区
const unsigned int MAX_PARAM_NUM             = 5000;			//参数最大限度，向辅助决策发送参数时，当参数队列超过最大值时，开始丢弃旧的参数
const unsigned int MAX_PARAM_NAME_LEN        = 100;				//管道数据尾码
const unsigned int FRAME_END_CODE            = 0x89abcdef;		//帧尾码
const unsigned short CHILD_FRAME_END_CODE    = 0xffff;			//子帧尾码
const unsigned short PIPE_DATA_END_CODE      = 0xfffe;			//管道数据尾码
const unsigned int MAX_REC_FILE_LEN          = 30000000;		//记盘文件大小

const unsigned short MAX_THREAD_NUM            = 50;				//最大线程数

const unsigned short PDXP_VERSION = 0x80; //PDXP包版本号,PDXP包第一个字节

typedef unsigned short u_int16_t;
typedef unsigned int u_int32_t;
typedef unsigned char u_int8_t;


#endif

