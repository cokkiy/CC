#TARGET      = compassplugin
#QT         += widgets designer-private axcontainer

#PLUGIN_CLASS_NAME = compassPlugin
#include(../plugins.pri)
#include(compass.pri)

#SOURCES += compassplugin.cpp \

#HEADERS += compassplugin.h \

#OTHER_FILES += activeqt.json




CONFIG      += plugin debug_and_release
TARGET      = $$qtLibraryTarget(compassplugin)
TEMPLATE    = lib

HEADERS     = compassplugin.h
SOURCES     = compassplugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L.

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target

include(compass.pri)
