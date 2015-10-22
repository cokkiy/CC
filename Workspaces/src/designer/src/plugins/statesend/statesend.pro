CONFIG      += plugin debug_and_release
TARGET      = statesendplugin
QT         += widgets #designer-private #axcontainer
QT         += network
#begin andrew ,20150310
#designer-private designercomponents-private
load(qt_build_paths)
DESTDIR = $$MODULE_BASE_OUTDIR/bin
MODULE_BASE_INDIR = $$dirname(_QMAKE_CONF_)
MODULE_BASE_OUTDIR = $$shadowed($$MODULE_BASE_INDIR)
!isEmpty(MODULE_BASE_DIR): MODULE_SYNCQT_DIR = $$MODULE_BASE_DIR    # compat for webkit
isEmpty(MODULE_SYNCQT_DIR): MODULE_SYNCQT_DIR = $$MODULE_BASE_INDIR
isEmpty(MODULE_QMAKE_OUTDIR): MODULE_QMAKE_OUTDIR = $$MODULE_BASE_OUTDIR
win32 {
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerBase.dll
}
unix {
    LIBS += -L$$MODULE_BASE_OUTDIR/lib/ -lQt5DesignerBase -lnet
}
mac {

}
#INCLUDEPATH += $$PWD/../../../../../../include
INCLUDEPATH += $$MODULE_BASE_INDIR/include/QtDesigner/5.3.1 \
    $$MODULE_BASE_INDIR/include/QtDesigner \
    $$MODULE_BASE_INDIR/include/QtDesigner/5.3.1/QtDesigner \
    $$MODULE_BASE_INDIR/include
#end andrew ,20150310
PLUGIN_CLASS_NAME = StateSendPlugin
include(../plugins.pri)
HEADERS     = statesendplugin.h
SOURCES     = statesendplugin.cpp
RESOURCES   = icons.qrc
include(statesend.pri)

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer

} else {
    CONFIG += designer
}

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target
