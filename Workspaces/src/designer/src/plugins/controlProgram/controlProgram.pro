CONFIG      += plugin debug_and_release
TARGET      = $$qtLibraryTarget(controlprogramplugin)
TEMPLATE    = lib

HEADERS     = controlprogramplugin.h
SOURCES     = controlprogramplugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L.

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}

load(qt_build_paths)

DESTDIR = $$MODULE_BASE_OUTDIR/plugins/designer
INSTALLS    += target

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

include(controlprogram.pri)
