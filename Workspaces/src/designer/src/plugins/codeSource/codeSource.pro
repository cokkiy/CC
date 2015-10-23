CONFIG      += plugin debug_and_release
TARGET      = $$qtLibraryTarget(codeSourceplugin)
TEMPLATE    = lib

HEADERS     = codeSourceplugin.h \
    codesource.h
SOURCES     = codeSourceplugin.cpp \
    codesource.cpp
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
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerBase.dll
    LIBS += $$MODULE_BASE_OUTDIR/lib/net.dll
}
else:win32:CONFIG(debug, debug|release){
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerBased.dll
    LIBS += $$MODULE_BASE_OUTDIR/lib/netd.dll
}
unix {
    LIBS += -L$$MODULE_BASE_OUTDIR/lib/ -lQt5DesignerBase -lnet
}
mac {

}
include(codeSource.pri)

OTHER_FILES += \
    codesource.pri
