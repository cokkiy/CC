#-------------------------------------------------
#
# Project created by QtCreator 2015-03-24T02:02:39
#
#-------------------------------------------------

QT       -= gui
QT       += core
QT       += network
QT       += xml

TARGET = net
TEMPLATE = lib
DEFINES += NET_LIBRARY
load(qt_build_config)
load(qt_module)
DESTDIR = $$MODULE_BASE_OUTDIR/lib
#begin andrew ,20150324
win32:CONFIG(release, debug|release){
    LIBS += $$MODULE_BASE_OUTDIR/lib/WS2_32.lib
}
else:win32:CONFIG(debug, debug|release){
    LIBS += $$MODULE_BASE_OUTDIR/lib/WS2_32.lib
}

#INCLUDEPATH += $$MODULE_BASE_OUTDIR/lib/
#DEPENDPATH += $$MODULE_BASE_OUTDIR/lib/
INCLUDEPATH += #$$MODULE_BASE_INDIR/include/QtDesigner/5.3.1 \
    #$$MODULE_BASE_INDIR/include/QtDesigner \
    #$$MODULE_BASE_INDIR/include/QtDesigner/5.3.1/QtDesigner \
    $$MODULE_BASE_INDIR/include
#end andrew ,20150324
SOURCES += \
       selfshare/src/buffer/buffer.cpp \
       selfshare/src/config/config.cpp \
       selfshare/src/historyparam/historyparam.cpp \
       selfshare/src/historyparambuffer/historyparambuffer.cpp \
       selfshare/src/netbuffer/netbuffer.cpp \
       selfshare/src/thread/thread.cpp \
       selfshare/src/xmlproc/xmlproc.cpp \
       selfshare/src/zxparambuffer/zxparambuffer.cpp \
       main/src/network/network.cpp \
       main/src/recvinfothread/recvinfothread.cpp \
       main/src/infoprocthread/infoprocthread.cpp \
       LFormula.cpp \
       abstractparam.cpp \
    abstractdatacenter.cpp \
    abstractinformation.cpp \
    abstractnet.cpp \
    datacenterinstance.cpp \
    datacentertest.cpp \
    informationinstance.cpp \
    netcomponents.cpp \
    netinstance.cpp \    
    selfshare/src/config/xelement.cpp \
    selfshare/src/config/netconfig.cpp \
    selfshare/src/config/xattribute.cpp \
    selfshare/src/config/xdocument.cpp \
    selfshare/src/config/channelselectconfig.cpp \
    taskinstance.cpp \
    abstracttask.cpp \
    abstractstation.cpp \
    stationinstance.cpp \
    selfshare/src/config/stationconfig.cpp \
    showtype/dataresultstring.cpp \
    showtype/dateresultstring.cpp \
    showtype/numericalvalueresultstring.cpp \
    showtype/timeresultstring.cpp\
    C3I/CParamInfo.cpp \
    C3I/CParamInfoRT.cpp \
    C3IFrame.cpp \
    abstracttimer.cpp \
    timerinstance.cpp \
    PDXPFrame.cpp \
    showtype/coderesultstring.cpp \
    GC.cpp \
    getMemorySize.cpp \
    getRSS.cpp \
    HistoryBufferManager.cpp \
    SimpleLogger.cpp \
    C3I/paramstyle/paramstyleconfig.cpp \
    C3I/paramstyle/paramstyle.cpp \
    C3I/paramstyle/paramindex.cpp

HEADERS += \
        net_global.h \
       selfshare/src/baseobject.h \
       selfshare/src/const.h \
       selfshare/src/datastruct.h \
       selfshare/src/OS_Def.h \
       selfshare/src/buffer/buffer.h \
       selfshare/src/config/config.h \
       selfshare/src/historyparam/historyparam.h \
       selfshare/src/historyparambuffer/historyparambuffer.h \
       selfshare/src/netbuffer/netbuffer.h \
       selfshare/src/thread/thread.h \
       selfshare/src/xmlproc/xmlproc.h \
       selfshare/src/zxparambuffer/zxparambuffer.h \
       main/src/network/network.h \
       main/src/recvinfothread/recvinfothread.h \
       main/src/infoprocthread/infoprocthread.h \
       LFormula.h \
       abstractparam.h \
    abstractdatacenter.h \
    abstractinformation.h \
    abstractnet.h \
    datacenterinstance.h \
    datacentertest.h \
    informationinstance.h \
    netcomponents.h \
    abstractudpclient.h \    
    selfshare/src/config/netconfig.h \
    selfshare/src/config/xattribute.h \
    selfshare/src/config/xelement.h \
    selfshare/src/config/xdocument.h\
    netinstance.h \
    selfshare/src/config/channelselectconfig.h \
    taskinstance.h \
    abstracttask.h \
    abstractstation.h \
    stationinstance.h \
    selfshare/src/config/stationconfig.h \
    showtype/dataresultstring.h \
    showtype/dateresultstring.h \
    showtype/numericalvalueresultstring.h \
    showtype/timeresultstring.h\
    C3I/CParamInfo.h \
    C3I/CParamInfoRT.h \
    C3I/stru.h \
    C3IFrame.h \
    abstracttimer.h \
    timerinstance.h \
    PDXPFrame.h \
    showtype/coderesultstring.h \
    GC.h \
    GCWrapper.h \
    HistoryBufferManager.h \
    SimpleLogger.h \
    C3I/paramstyle/paramstyleconfig.h \
    C3I/paramstyle/paramstyle.h \
    C3I/paramstyle/paramindex.h


unix {
    target.path = /usr/lib
    INSTALLS += target
}
