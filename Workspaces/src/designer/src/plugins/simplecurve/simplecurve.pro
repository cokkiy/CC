CONFIG      += plugin debug_and_release
TARGET      = $$qtLibraryTarget(simplecurveplugin)
TEMPLATE    = lib

HEADERS     = simplecurveplugin.h
SOURCES     = simplecurveplugin.cpp
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
include(simplecurve.pri)




#TARGET      = simplecurveplugin
#QT         += widgets #designer-private #axcontainer
#qtHaveModule(printsupport): QT += printsupport
##begin andrew ,20150316
##designer-private designercomponents-private
#load(qt_build_paths)
#DESTDIR = $$MODULE_BASE_OUTDIR/bin
#MODULE_BASE_INDIR = $$dirname(_QMAKE_CONF_)
#MODULE_BASE_OUTDIR = $$shadowed($$MODULE_BASE_INDIR)
#!isEmpty(MODULE_BASE_DIR): MODULE_SYNCQT_DIR = $$MODULE_BASE_DIR    # compat for webkit
#isEmpty(MODULE_SYNCQT_DIR): MODULE_SYNCQT_DIR = $$MODULE_BASE_INDIR
#isEmpty(MODULE_QMAKE_OUTDIR): MODULE_QMAKE_OUTDIR = $$MODULE_BASE_OUTDIR
#win32 {
#    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerBased.dll
#    LIBS += $$MODULE_BASE_OUTDIR/lib/netd.dll
#}
#unix {
#    LIBS += -L$$MODULE_BASE_OUTDIR/lib/ -lQt5DesignerBase -lnet
#}
#mac {

#}
##INCLUDEPATH += $$PWD/../../../../../../include
#INCLUDEPATH += $$MODULE_BASE_INDIR/include/QtDesigner/5.3.1 \
#    $$MODULE_BASE_INDIR/include/QtDesigner \
#    $$MODULE_BASE_INDIR/include/QtDesigner/5.3.1/QtDesigner \
#    $$MODULE_BASE_INDIR/include
##end andrew ,20150316
#PLUGIN_CLASS_NAME = SimpleCurvePlugin
#include(../plugins.pri)
#HEADERS     = simplecurveplugin.h
#SOURCES     = simplecurveplugin.cpp
#RESOURCES   = icons.qrc
#include(simplecurve.pri)
