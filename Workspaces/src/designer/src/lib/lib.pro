MODULE = designer

TARGET = QtDesignerBase
QT = core-private gui-private widgets-private xml
MODULE_PLUGIN_TYPES = designer
MODULE_CONFIG = designer_defines
load(qt_module)

INCLUDEPATH += $$MODULE_BASE_INDIR/include/QtDesigner/5.3.1 \
    $$MODULE_BASE_INDIR/include/QtDesigner \
    $$MODULE_BASE_INDIR/include/QtDesigner/5.3.1/QtDesigner \
    $$MODULE_BASE_INDIR/include
DEFINES += \
    QDESIGNER_SDK_LIBRARY \
    QDESIGNER_EXTENSION_LIBRARY \
    QDESIGNER_UILIB_LIBRARY \
    QDESIGNER_SHARED_LIBRARY

static:DEFINES += QT_DESIGNER_STATIC

include(extension/extension.pri)
include(sdk/sdk.pri)
include(shared/shared.pri)
include(uilib/uilib.pri)
PRECOMPILED_HEADER=lib_pch.h
