CONFIG      += plugin debug_and_release
TARGET      = $$qtLibraryTarget(tableplugin)
TEMPLATE    = lib

HEADERS     = tableplugin.h \
    ../../components/propertyeditor/simpletable/tablecell.h \
    ../../components/propertyeditor/simpletable/tableproperty.h
SOURCES     = tableplugin.cpp \
    ../../components/propertyeditor/simpletable/tablecell.cpp \
    ../../components/propertyeditor/simpletable/tableproperty.cpp
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
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerComponentsBase.dll
}
else:win32:CONFIG(debug, debug|release){
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerBased.dll
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerComponentsBased.dll
}
else:unix:LIBS += -L$$MODULE_BASE_OUTDIR/lib/ -lQt5DesignerBase -lQt5DesignerComponentsBase -lnet

include(table.pri)
