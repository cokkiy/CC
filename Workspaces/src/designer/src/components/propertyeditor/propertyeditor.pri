INCLUDEPATH += $$PWD

# --- Property browser is also linked into the designer_shared library.
#     Avoid conflict when linking statically
contains(CONFIG, static) {
    INCLUDEPATH *= ../../../../shared/qtpropertybrowser
    INCLUDEPATH *= ../../../../shared/qtgradienteditor
} else {
    include(../../../../shared/qtpropertybrowser/qtpropertybrowser.pri)
    include(../../../../shared/qtgradienteditor/qtcolorbutton.pri)
}

FORMS += $$PWD/paletteeditor.ui \
    $$PWD/stringlisteditor.ui \
    $$PWD/previewwidget.ui \
    $$PWD/newdynamicpropertydialog.ui \
    ../propertyeditor/singlestatedialog.ui \
    ../propertyeditor/selectmultiparamdlg.ui \
    ../propertyeditor/tableitemset.ui \
    ../propertyeditor/tableset.ui \
    ../propertyeditor/graphdialog/graphdialog.ui

HEADERS += $$PWD/propertyeditor.h \
    $$PWD/designerpropertymanager.h \
    $$PWD/paletteeditor.h \
    $$PWD/paletteeditorbutton.h \
    $$PWD/stringlisteditor.h \
    $$PWD/stringlisteditorbutton.h \
    $$PWD/previewwidget.h \
    $$PWD/previewframe.h \
    $$PWD/newdynamicpropertydialog.h \
    $$PWD/brushpropertymanager.h \
    $$PWD/fontpropertymanager.h \
    $$PWD/selectparamdialog.h \
    ../propertyeditor/singlestatelineeditdelegate.h \
    ../propertyeditor/singlestatespindelegate.h \
    ../propertyeditor/singlestatedialog.h \
    ../propertyeditor/selectmultiparamdlg.h \
    ../propertyeditor/tablecell.h \
    ../propertyeditor/tableitemset.h \
    ../propertyeditor/tablepropertysetDlg.h \
    ../propertyeditor/tableset.h \
    ../propertyeditor/graphdialog/graphdialog.h

SOURCES += $$PWD/propertyeditor.cpp \
    $$PWD/designerpropertymanager.cpp \
    $$PWD/paletteeditor.cpp \
    $$PWD/paletteeditorbutton.cpp \
    $$PWD/stringlisteditor.cpp \
    $$PWD/stringlisteditorbutton.cpp \
    $$PWD/previewwidget.cpp \
    $$PWD/previewframe.cpp \
    $$PWD/newdynamicpropertydialog.cpp \
    $$PWD/brushpropertymanager.cpp \
    $$PWD/fontpropertymanager.cpp \
    $$PWD/selectparamdialog.cpp \
    ../propertyeditor/singlestatelineeditdelegate.cpp \
    ../propertyeditor/singlestatespindelegate.cpp \
    ../propertyeditor/singlestatedialog.cpp\
    ../propertyeditor/selectmultiparamdlg.cpp \
    ../propertyeditor/tablecell.cpp \
    ../propertyeditor/tableitemset.cpp \
    ../propertyeditor/tablepropertysetDlg.cpp \
    ../propertyeditor/tableset.cpp\
    ../propertyeditor/graphdialog/graphdialog.cpp \

HEADERS += \
    $$PWD/propertyeditor_global.h \
    $$PWD/qlonglongvalidator.h

SOURCES += $$PWD/qlonglongvalidator.cpp

RESOURCES += $$PWD/propertyeditor.qrc
