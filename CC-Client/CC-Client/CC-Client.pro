#-------------------------------------------------
#
# Project created by QtCreator 2015-10-01T11:05:36
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CC-Client
TEMPLATE = app

#c++ 11 supports
QMAKE_CXXFLAGS += -std=c++11
#QMAKE_CXXFLAGS += -std=c++14

# Ice lib
unix:LIBS += -L/opt/Ice-3.6.1/lib64/c++11 -lIce -lIceUtil

VER_MAJ=1
VER_MIN=1
VER_PAT=4367

#如果是中标麒麟，则定义Neolnux，版本太低，需要特殊处理
DEFINES += NeoLinux

INCLUDEPATH += generated/ \
            /opt/Ice-3.6.1/include \

include(/opt/qwt-6.1.3/features/qwt.prf)

SOURCES += main.cpp\
        mainwindow.cpp \
    cpuplot/counterpiemarker.cpp \
    generated/AppControlParameter.cpp \
    generated/IController.cpp \
    generated/IStationStateReceiver.cpp \
    CompareOperation.cpp \
    defaultappprocdialog.cpp \
    detaildialog.cpp \
    editstationdialog.cpp \
    executcounting.cpp \
    filterbuilddialog.cpp \
    FilterOperations.cpp \
    floatingmenu.cpp \
    monitor.cpp \
    NetworkInterface.cpp \
    option.cpp \
    qtdispatcher.cpp \
    recvfiledialog.cpp \
    recvfilethread.cpp \
    screenimagedialog.cpp \
    sendfiledialog.cpp \
    sendfilethread.cpp \
    setintervaldialog.cpp \
    stationbar.cpp \
    stationdetaildialog.cpp \
    StationInfo.cpp \
    StationList.cpp \
    stationmanager.cpp \
    StationStateReceiver.cpp \
    stationtablemodel.cpp \
    xattribute.cpp \
    xdocument.cpp \
    xelement.cpp \
    processdialog.cpp \
    filebrowserdialog.cpp \
    batchcapturedialog.cpp \
    batchcaputrethread.cpp \
    aboutdialog.cpp \
    updatemanager.cpp

HEADERS  += mainwindow.h \
    selectcomputerdialog.h \
    cpuplot/counterpiemarker.h \
    cpuplot/cpustat.h \
    cpuplot/plotpart.h \
    generated/AppControlParameter.h \
    generated/IController.h \
    generated/IStationStateReceiver.h \
    CompareOperation.h \
    defaultappprocdialog.h \
    detaildialog.h \
    editstationdialog.h \
    executcounting.h \
    filterbuilddialog.h \
    FilterOperations.h \
    floatingmenu.h \
    monitor.h \
    NetworkInterface.h \
    option.h \
    qtdispatcher.h \
    recvfiledialog.h \
    recvfilethread.h \
    resource.h \
    screenimagedialog.h \
    sendfiledialog.h \
    sendfilethread.h \
    setintervaldialog.h \
    stationbar.h \
    stationdetaildialog.h \
    StationInfo.h \
    StationList.h \
    stationmanager.h \
    StationStateReceiver.h \
    stationtablemodel.h \
    xattribute.h \
    xdocument.h \
    xelement.h \
    processdialog.h \
    filebrowserdialog.h \
    batchcapturedialog.h \
    batchcaputrethread.h \
    aboutdialog.h \
    updatemanager.h

FORMS    += mainwindow.ui \
    defaultappprocdialog.ui \
    detaildialog.ui \
    filterBuildDialog.ui \
    floatingmenu.ui \
    recvfiledialog.ui \
    screenimagedialog.ui \
    setintervaldialog.ui \
    stationdetaildialog.ui \
    sendfiledialog.ui \
    editstationdialog.ui \
    processdialog.ui \
    filebrowserdialog.ui \
    batchcapturedialog.ui \
    aboutdialog.ui

RESOURCES += \
    cc-client.qrc

SUBDIRS += \
    cpuplot/cpuplot.pro

OTHER_FILES += \
    cc-client_zh.ts \
    Config.Server
