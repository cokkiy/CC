//根据是什么系统定义什么系统的宏
#include <qglobal.h>
#if defined(Q_OS_WIN)
#  define WIN
#elif !defined(Q_OS_WIN)
#  define LINUX
#endif
