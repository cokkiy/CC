MODULE = designercomponents
TARGET = QtDesignerComponentsBase
QT = core gui-private widgets-private
#designer-private

QT_PRIVATE = xml
CONFIG += internal_module
load(qt_build_config)
load(qt_module)
#begin andrew ,20150310
win32:CONFIG(release, debug|release){
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerBase.dll
    LIBS += $$MODULE_BASE_OUTDIR/lib/net.dll
}
else:win32:CONFIG(debug, debug|release){
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerBased.dll
    LIBS += $$MODULE_BASE_OUTDIR/lib/netd.dll
}
else:unix:LIBS += -L$$MODULE_BASE_OUTDIR/lib/ -lQt5DesignerBase -lnet
#INCLUDEPATH += $$MODULE_BASE_OUTDIR/lib/
#DEPENDPATH += $$MODULE_BASE_OUTDIR/lib/
INCLUDEPATH += $$MODULE_BASE_INDIR/include/QtDesigner/5.3.1 \
    $$MODULE_BASE_INDIR/include/QtDesigner \
    $$MODULE_BASE_INDIR/include/QtDesigner/5.3.1/QtDesigner \
    $$MODULE_BASE_INDIR/include
#end andrew ,20150310

# QtDesignerComponents uses
DEFINES += QT_STATICPLUGIN
DEFINES += QDESIGNER_COMPONENTS_LIBRARY

static:DEFINES += QT_DESIGNER_STATIC

# Override qt_targets
QMAKE_TARGET_PRODUCT = Designer
QMAKE_TARGET_DESCRIPTION = Graphical user interface designer.

SOURCES += qdesigner_components.cpp \

INCLUDEPATH += . .. \
    $$PWD/../../lib/components \
    $$PWD/../../lib/sdk \
    $$PWD/../../lib/extension \
    $$PWD/../../lib/shared

include(../propertyeditor/propertyeditor.pri)
include(../objectinspector/objectinspector.pri)
include(../signalsloteditor/signalsloteditor.pri)
include(../formeditor/formeditor.pri)
include(../widgetbox/widgetbox.pri)
include(../buddyeditor/buddyeditor.pri)
include(../taskmenu/taskmenu.pri)
include(../tabordereditor/tabordereditor.pri)
include(../imageexplorer/imageexplorer.pri)
PRECOMPILED_HEADER= lib_pch.h

# MinGW GCC cc1plus crashes when using precompiled header
# Date Checked: 3rd September 2012
#
# Compilers known to be affected:
# * MinGW-builds GCC 4.6.3 64-bit
# * MinGW-builds GCC 4.7.1 64-bit
# * MinGW-w64 GCC 4.7.1 64-bit (rubenvb)
mingw:CONFIG -= precompile_header

