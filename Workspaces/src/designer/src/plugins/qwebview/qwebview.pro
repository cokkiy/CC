TARGET      = qwebview
QT         += widgets webkitwidgets

PLUGIN_CLASS_NAME = QWebViewPlugin
include(../plugins.pri)

SOURCES += qwebview_plugin.cpp \
    zxwebview.cpp
HEADERS += qwebview_plugin.h \
    zxwebview.h
RESOURCES += qwebview_plugin.qrc
OTHER_FILES += qwebview.json
