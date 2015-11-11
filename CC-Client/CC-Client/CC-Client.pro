#-------------------------------------------------
#
# Project created by QtCreator 2015-10-01T11:05:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CC-Client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    selectcomputerdialog.cpp

HEADERS  += mainwindow.h \
    selectcomputerdialog.h

FORMS    += mainwindow.ui \
    selectcomputerdialog.ui

RESOURCES += \
    cc-client.qrc
