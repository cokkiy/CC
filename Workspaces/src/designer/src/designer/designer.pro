QT += core-private widgets xml network
#designer-private designercomponents-private
qtHaveModule(printsupport): QT += printsupport

INCLUDEPATH += \
    ../lib/sdk \
    ../lib/extension \
    ../lib/shared \
    extra

RESOURCES += designer.qrc

contains(QT_CONFIG, static) {
    DEFINES += QT_DESIGNER_STATIC
}

include(../../../shared/fontpanel/fontpanel.pri)
include(../../../shared/qttoolbardialog/qttoolbardialog.pri)

QMAKE_DOCS = $$PWD/doc/qtdesigner.qdocconf

HEADERS += \
    qdesigner.h \
    qdesigner_toolwindow.h \
    qdesigner_formwindow.h \
    qdesigner_workbench.h \
    qdesigner_settings.h \
    qdesigner_actions.h \
    qdesigner_server.h \
    qdesigner_appearanceoptions.h \
    saveformastemplate.h \
    newform.h \
    versiondialog.h \
    designer_enums.h \
    appfontdialog.h \
    preferencesdialog.h \
    assistantclient.h \
    mainwindow.h

SOURCES += main.cpp \
    qdesigner.cpp \
    qdesigner_toolwindow.cpp \
    qdesigner_formwindow.cpp \
    qdesigner_workbench.cpp \
    qdesigner_settings.cpp \
    qdesigner_server.cpp \
    qdesigner_actions.cpp \
    qdesigner_appearanceoptions.cpp \
    saveformastemplate.cpp \
    newform.cpp \
    versiondialog.cpp \
    appfontdialog.cpp \
    preferencesdialog.cpp \
    assistantclient.cpp \
    mainwindow.cpp

PRECOMPILED_HEADER=qdesigner_pch.h

FORMS += saveformastemplate.ui \
    preferencesdialog.ui \
    qdesigner_appearanceoptions.ui

win32 {
   RC_FILE      = designer.rc
}

mac {
    ICON = designer.icns
    QMAKE_INFO_PLIST = Info_mac.plist
    TARGET = Designer
    FILETYPES.files = uifile.icns
    FILETYPES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += FILETYPES
}

unix:!mac:LIBS += -lm

#load(qt_app)
TEMPLATE = app

load(qt_build_paths)
DESTDIR = $$MODULE_BASE_OUTDIR/bin
MODULE_BASE_INDIR = $$dirname(_QMAKE_CONF_)
MODULE_BASE_OUTDIR = $$shadowed($$MODULE_BASE_INDIR)
!isEmpty(MODULE_BASE_DIR): MODULE_SYNCQT_DIR = $$MODULE_BASE_DIR    # compat for webkit
isEmpty(MODULE_SYNCQT_DIR): MODULE_SYNCQT_DIR = $$MODULE_BASE_INDIR
isEmpty(MODULE_QMAKE_OUTDIR): MODULE_QMAKE_OUTDIR = $$MODULE_BASE_OUTDIR

#begin andrew ,20150310
win32:CONFIG(release, debug|release){
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerBase.dll
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerComponentsBase.dll
}
else:win32:CONFIG(debug, debug|release){
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerBased.dll
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5DesignerComponentsBased.dll
    #LIBS += -L$$MODULE_BASE_OUTDIR/lib/ -lQt5Designer -lQt5DesignerComponents -lQt5UiTools -lQt5PrintSupport  -lQt5Widgets -lQt5Network -lQt5Xml -lQt5Gui -lQt5Core -lGL -lpthread

}
else:unix:LIBS += -L$$MODULE_BASE_OUTDIR/lib/ -lQt5DesignerBase -lQt5DesignerComponentsBase -lnet
#INCLUDEPATH += $$MODULE_BASE_OUTDIR/lib/
#DEPENDPATH += $$MODULE_BASE_OUTDIR/lib/
INCLUDEPATH += $$MODULE_BASE_INDIR/include/QtDesigner/5.3.1 \
    $$MODULE_BASE_INDIR/include/QtDesigner \
    $$MODULE_BASE_INDIR/include/QtDesigner/5.3.1/QtDesigner \
    $$MODULE_BASE_INDIR/include
#end andrew ,20150310

#win32:CONFIG(release, debug|release): LIBS += -L$$MODULE_BASE_OUTDIR/lib/ -lQt5Designer -lQt5DesignerComponents -lQt5UiTools -lQt5PrintSupport  -lQt5Widgets -lQt5Network -lQt5Xml -lQt5Gui -lQt5Core -lGL -lpthread
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$MODULE_BASE_OUTDIR/lib/ -lQt5Designer -lQt5DesignerComponents -lQt5UiTools -lQt5PrintSupport  -lQt5Widgets -lQt5Network -lQt5Xml -lQt5Gui -lQt5Core -lGL -lpthread
#else:unix: LIBS += -L$$OUT_PWD/../lib/ -lQt5Designer
isEmpty(QMAKE_INFO_PLIST): CONFIG -= app_bundle

# This decreases the binary size for tools if statically linked
QMAKE_LFLAGS += $$QMAKE_LFLAGS_GCSECTIONS

host_build: QT -= gui   # no host tool will ever use gui
host_build:force_bootstrap {
    #!build_pass: CONFIG += release
    contains(QT, core(-private)?|xml) {
        QT -= core core-private xml
        QT += bootstrap-private
    }
    target.path = $$[QT_HOST_BINS]
} else {
    #!build_pass:contains(QT_CONFIG, debug_and_release):contains(QT_CONFIG, build_all): CONFIG += release
    target.path = $$[QT_INSTALL_BINS]
}
INSTALLS += target

#load(qt_targets)
#QMAKE_TARGET_COMPANY = Digia Plc and/or its subsidiary(-ies)
#QMAKE_TARGET_PRODUCT = Qt5
#QMAKE_TARGET_DESCRIPTION = C++ application development framework.
#QMAKE_TARGET_COPYRIGHT = Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).


#load(qt_common)
QMAKE_DIR_REPLACE_SANE += DESTDIR
CONFIG -= debug_and_release_target

contains(QT_CONFIG, c++11): CONFIG += c++11
contains(TEMPLATE, .*lib) {
    # module and plugins
    contains(QT_CONFIG, reduce_exports): CONFIG += hide_symbols
    unix:contains(QT_CONFIG, reduce_relocations): CONFIG += bsymbolic_functions
    contains(QT_CONFIG, largefile): CONFIG += largefile
    contains(QT_CONFIG, separate_debug_info): CONFIG += separate_debug_info

    !isEmpty(_QMAKE_SUPER_CACHE_): \
        rplbase = $$dirname(_QMAKE_SUPER_CACHE_)/[^/][^/]*
    else: \
        rplbase = $$MODULE_BASE_OUTDIR
    host_build: \
        qt_libdir = $$[QT_HOST_LIBS]
    else: \
        qt_libdir = $$[QT_INSTALL_LIBS/raw]
    contains(QMAKE_DEFAULT_LIBDIRS, $$qt_libdir) {
        lib_replace.match = "[^ ']*$$rplbase/lib"
        lib_replace.replace =
    } else {
        lib_replace.match = $$rplbase/lib
        lib_replace.replace = $$qt_libdir
    }
    lib_replace.CONFIG = path
    QMAKE_PRL_INSTALL_REPLACE += lib_replace
}

warnings_are_errors:warning_clean {
    # If the module declares that it has does its clean-up of warnings, enable -Werror.
    # This setting is compiler-dependent anyway because it depends on the version of the
    # compiler.
    clang {
        # Apple clang 4.0-4.2,5.0
        # Regular clang 3.3 & 3.4
        apple_ver = $${QT_APPLE_CLANG_MAJOR_VERSION}.$${QT_APPLE_CLANG_MINOR_VERSION}
        reg_ver = $${QT_CLANG_MAJOR_VERSION}.$${QT_CLANG_MINOR_VERSION}
        contains(apple_ver, "4\\.[012]|5\\.0")|contains(reg_ver, "3\\.[34]") {
            QMAKE_CXXFLAGS_WARN_ON += -Werror -Wno-error=\\$${LITERAL_HASH}warnings -Wno-error=deprecated-declarations $$WERROR

            # glibc's bswap_XX macros use the "register" keyword
            linux:equals(reg_ver, "3.4"): QMAKE_CXXFLAGS_WARN_ON += -Wno-error=deprecated-register
        }
    } else:intel_icc:linux {
        # Intel CC 13.0 - 14.0, on Linux only
        ver = $${QT_ICC_MAJOR_VERSION}.$${QT_ICC_MINOR_VERSION}
        linux:contains(ver, "(13\\.|14\\.0)") {
            # 177: function "entity" was declared but never referenced
            #      (too aggressive; ICC reports even for functions created due to template instantiation)
            # 1224: #warning directive
            # 1478: function "entity" (declared at line N) was declared deprecated
            # 1881: argument must be a constant null pointer value
            #      (NULL in C++ is usually a literal 0)
            QMAKE_CXXFLAGS_WARN_ON += -Werror -ww177,1224,1478,1881 $$WERROR
        }
    } else:gcc:!clang:!intel_icc {
        # GCC 4.6-4.8
        ver = $${QT_GCC_MAJOR_VERSION}.$${QT_GCC_MINOR_VERSION}
        contains(ver, "4\\.[678]") {
            QMAKE_CXXFLAGS_WARN_ON += -Werror -Wno-error=cpp -Wno-error=deprecated-declarations $$WERROR

            # GCC prints this bogus warning, after it has inlined a lot of code
            # error: assuming signed overflow does not occur when assuming that (X + c) < X is always false
            QMAKE_CXXFLAGS_WARN_ON += -Wno-error=strict-overflow

            # Work-around for bug https://code.google.com/p/android/issues/detail?id=58135
            android: QMAKE_CXXFLAGS_WARN_ON += -Wno-error=literal-suffix
        }
    }
    unset(ver)
}
