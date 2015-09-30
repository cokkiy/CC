QT += widgets testlib

SOURCES = testformula.cpp

load(qt_build_paths)
DESTDIR = $$MODULE_BASE_OUTDIR/bin
MODULE_BASE_INDIR = $$dirname(_QMAKE_CONF_)
MODULE_BASE_OUTDIR = $$shadowed($$MODULE_BASE_INDIR)
!isEmpty(MODULE_BASE_DIR): MODULE_SYNCQT_DIR = $$MODULE_BASE_DIR    # compat for webkit
isEmpty(MODULE_SYNCQT_DIR): MODULE_SYNCQT_DIR = $$MODULE_BASE_INDIR
isEmpty(MODULE_QMAKE_OUTDIR): MODULE_QMAKE_OUTDIR = $$MODULE_BASE_OUTDIR

#begin andrew ,20150327
win32:CONFIG(release, debug|release){
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5net.dll
}
else:win32:CONFIG(debug, debug|release){
    LIBS += $$MODULE_BASE_OUTDIR/lib/Qt5netd.dll
}
else:unix:LIBS += -L$$MODULE_BASE_OUTDIR/lib/ -lnet
#INCLUDEPATH += $$MODULE_BASE_OUTDIR/lib/
#DEPENDPATH += $$MODULE_BASE_OUTDIR/lib/
INCLUDEPATH += $$MODULE_BASE_INDIR/include/QtDesigner/5.3.1 \
    $$MODULE_BASE_INDIR/include/QtDesigner \
    $$MODULE_BASE_INDIR/include/QtDesigner/5.3.1/QtDesigner \
    $$MODULE_BASE_INDIR/include
#end andrew ,20150327

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/net_test
#INSTALLS += target
