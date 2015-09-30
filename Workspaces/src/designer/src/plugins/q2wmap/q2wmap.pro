CONFIG      += plugin debug_and_release
TARGET      = $$qtLibraryTarget(q2wmapplugin)
TEMPLATE    = lib

HEADERS     = q2wmapplugin.h
SOURCES     = q2wmapplugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L.
qtHaveModule(printsupport): QT += printsupport
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}
#使用qt的编译路径
load(qt_build_paths)
#指定插件的存放路径
DESTDIR = $$MODULE_BASE_OUTDIR/plugins/designer
INSTALLS    += target
#加载指定的库文件
win32:CONFIG(release, debug|release){
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerBase.lib
    LIBS += $$MODULE_BASE_OUTDIR/lib/net.lib
}
else:win32:CONFIG(debug, debug|release){
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerBased.lib
    LIBS += $$MODULE_BASE_OUTDIR/lib/netd.lib
}
unix {
    LIBS += -L$$MODULE_BASE_OUTDIR/lib/ -lQt5DesignerBase -lnet
}
mac {

}
include(q2wmap.pri)
