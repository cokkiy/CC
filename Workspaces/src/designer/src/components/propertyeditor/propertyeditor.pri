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
    $$PWD/q2wmapobj/q2wmapobjdlg.ui \
    $$PWD/q2wmapobj/q2wmapobj.ui \
    $$PWD/selectmultiparamdlg.ui \
    $$PWD/selectsingleparamdialog.ui \
    $$PWD/tablepropertysetdialog.ui \
    $$PWD/tableitemset.ui\
    $$PWD/graphobj/graphobjdlg.ui \
    $$PWD/graphobj/graphobj.ui \
    $$PWD/q2wmapobj/q2wmapstaticelementdlg.ui \
    $$PWD/q2wmapobj/q2wmappolygondialog.ui \
    $$PWD/q2wmapobj/q2wmaplabeldlg.ui \
    $$PWD/q2wmapobj/q2wmapcircledlg.ui \
    $$PWD/configshowtype/configshowtypedialog.ui

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
    ../propertyeditor/singlestatelineeditdelegate.h \
    ../propertyeditor/singlestatespindelegate.h \
    ../propertyeditor/singlestatedialog.h \
    $$PWD/q2wmapobj/q2wmapobj.h \
    $$PWD/q2wmapobj/q2wmapobjdlg.h \
    $$PWD/simpletable/tablepropertysetDlg.h \
    $$PWD/simpletable/tableproperty.h \
    $$PWD/simpletable/tableitemset.h \
    $$PWD/simpletable/tablecell.h \
    $$PWD/paramselect/selectparamdialog.h \
    $$PWD/paramselect/selectmultiparamdlg.h\
    $$PWD/graphobj/graphobj.h \
    $$PWD/graphobj/graphobjdlg.h \
    $$PWD/q2wmapobj/q2wmapstaticelementdlg.h \
    $$PWD/q2wmapobj/q2wmappolygondialog.h \
    $$PWD/q2wmapobj/q2wmaplabeldlg.h \
    $$PWD/q2wmapobj/q2wmapcircledlg.h \
    $$PWD/q2wmapobj/q2wmapelementdlg.h \
    $$PWD/configshowtype/configshowtypedialog.h

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
    ../propertyeditor/singlestatelineeditdelegate.cpp \
    ../propertyeditor/singlestatespindelegate.cpp \
    ../propertyeditor/singlestatedialog.cpp\
    $$PWD/q2wmapobj/q2wmapobj.cpp \
    $$PWD/q2wmapobj/q2wmapobjdlg.cpp \
    $$PWD/simpletable/tablepropertysetDlg.cpp \
    $$PWD/simpletable/tableproperty.cpp \
    $$PWD/simpletable/tableitemset.cpp \
    $$PWD/simpletable/tablecell.cpp \
    $$PWD/paramselect/selectparamdialog.cpp \
    $$PWD/paramselect/selectmultiparamdlg.cpp\
    $$PWD/graphobj/graphobj.cpp \
    $$PWD/graphobj/graphobjdlg.cpp \
    $$PWD/q2wmapobj/q2wmapstaticelementdlg.cpp \
    $$PWD/q2wmapobj/q2wmappolygondialog.cpp \
    $$PWD/q2wmapobj/q2wmaplabeldlg.cpp \
    $$PWD/q2wmapobj/q2wmapcircledlg.cpp \
    $$PWD/q2wmapobj/q2wmapelementdlg.cpp \
    $$PWD/configshowtype/configshowtypedialog.cpp

HEADERS += \
    $$PWD/propertyeditor_global.h \
    $$PWD/qlonglongvalidator.h

SOURCES += $$PWD/qlonglongvalidator.cpp

RESOURCES += $$PWD/propertyeditor.qrc
