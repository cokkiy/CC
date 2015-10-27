#load(qt_parts)

cache()

#load(qt_build_config)
#!contains(QMAKE_INTERNAL_INCLUDED_FILES, .*qmodule\\.pri) {
#    QMAKE_QT_MODULE = $$[QT_HOST_DATA/get]/mkspecs/qmodule.pri
#    !exists($$QMAKE_QT_MODULE)|!include($$QMAKE_QT_MODULE, "", true) {
#        error("Cannot load qmodule.pri!")
#    } else {
#        debug(1, "Loaded qmodule.pri from ($$QMAKE_QT_MODULE)")
#    }
#} else {
#    debug(1, "Not loading qmodule.pri twice")
#}

PRECOMPILED_DIR = .pch
OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc
UI_DIR = .uic

QMAKE_DIR_REPLACE_SANE = PRECOMPILED_DIR OBJECTS_DIR MOC_DIR RCC_DIR UI_DIR

# force_independent can be set externally. prefix_build not.
!exists($$[QT_HOST_DATA]/.qmake.cache): \
    CONFIG += prefix_build force_independent

!build_pass:!isEmpty(_QMAKE_SUPER_CACHE_):force_independent {
    # When doing a -prefix build of top-level qt5/qt.pro, we need to announce
    # this repo's output dir to the other repos.
    MODULE_BASE_OUTDIR = $$shadowed($$dirname(_QMAKE_CONF_))
    !contains(QTREPOS, $$MODULE_BASE_OUTDIR): \
        cache(QTREPOS, add super, MODULE_BASE_OUTDIR)
    # This repo's module pris' location needs to be made known to qmake.
    isEmpty(MODULE_QMAKE_OUTDIR): MODULE_QMAKE_OUTDIR = $$MODULE_BASE_OUTDIR
    modpath = $$MODULE_QMAKE_OUTDIR/mkspecs/modules
    !contains(QMAKEMODULES, $$modpath): \
        cache(QMAKEMODULES, add super, modpath)
    unset(modpath)
}

mac {
   !isEmpty(QMAKE_RPATHDIR){
       CONFIG += absolute_library_soname
   }
}

cross_compile: \
    CONFIG += force_bootstrap

CONFIG += \
    create_prl link_prl \
    prepare_docs qt_docs_targets \
    no_private_qt_headers_warning QTDIR_build \
    # Qt modules get compiled without exceptions enabled by default.
    # However, testcases should be still built with exceptions.
    exceptions_off testcase_exceptions


defineTest(qtBuildPart) {
    bp = $$eval($$upper($$section(_QMAKE_CONF_, /, -2, -2))_BUILD_PARTS)
    isEmpty(bp): bp = $$QT_BUILD_PARTS
    contains(bp, $$1): return(true)
    return(false)
}

defineTest(qtNomakeTools) {
    qtBuildPart(tools): return()
    for (d, 1) {
        $${d}.CONFIG += no_default_target no_default_install
        export($${d}.CONFIG)
    }
}





TEMPLATE = subdirs

bp = $$eval($$upper($$TARGET)_BUILD_PARTS)
!isEmpty(bp): QT_BUILD_PARTS = $$bp

exists($$_PRO_FILE_PWD_/src/src.pro) {
    sub_src.subdir = src
    sub_src.target = sub-src
    SUBDIRS += sub_src

    exists($$_PRO_FILE_PWD_/tools/tools.pro) {
        sub_tools.subdir = tools
        sub_tools.target = sub-tools
        sub_tools.depends = sub_src
        # conditional treatment happens on a case-by-case basis
        SUBDIRS += sub_tools
    }
}

#exists($$_PRO_FILE_PWD_/examples/examples.pro) {
#    sub_examples.subdir = examples
#    sub_examples.target = sub-examples
#    contains(SUBDIRS, sub_src): sub_examples.depends = sub_src
#    examples_need_tools: sub_examples.depends += sub_tools
#    !contains(QT_BUILD_PARTS, examples): sub_examples.CONFIG = no_default_target no_default_install
#    SUBDIRS += sub_examples
#}

# Some modules still have these
exists($$_PRO_FILE_PWD_/demos/demos.pro) {
    sub_demos.subdir = demos
    sub_demos.target = sub-demos
    contains(SUBDIRS, sub_src): sub_demos.depends = sub_src
    examples_need_tools: sub_demos.depends += sub_tools
    !contains(QT_BUILD_PARTS, examples): sub_demos.CONFIG = no_default_target no_default_install
    SUBDIRS += sub_demos
}

#exists($$_PRO_FILE_PWD_/tests/tests.pro) {
#    sub_tests.subdir = tests
#    sub_tests.target = sub-tests
#    contains(SUBDIRS, sub_src): sub_tests.depends = sub_src   # The tests may have a run-time only dependency on other parts
#    tests_need_tools: sub_tests.depends += sub_tools
#    sub_tests.CONFIG = no_default_install
#    !contains(QT_BUILD_PARTS, tests) {
#        sub_tests.CONFIG += no_default_target
#    } else: !ios {
#        # Make sure these are there in case we need them
#        sub_tools.CONFIG -= no_default_target
#        sub_examples.CONFIG -= no_default_target
#        sub_demos.CONFIG -= no_default_target
#    }
#    SUBDIRS += sub_tests
#}

QT_BUILD_PARTS -= libs tools examples tests
!isEmpty(QT_BUILD_PARTS): warning("Unknown build part(s): $$QT_BUILD_PARTS")
