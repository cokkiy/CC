set(HEADER_FILES
${HEADER_FILES}
${CMAKE_CURRENT_SOURCE_DIR}/selectupgrademethoddlg.h
${CMAKE_CURRENT_SOURCE_DIR}/sendupgradefiledialog.h
${CMAKE_CURRENT_SOURCE_DIR}/sysupgradedialog.h
${CMAKE_CURRENT_SOURCE_DIR}/sysupgradewizard.h

PARENT_SCOPE
)

set(SOURCE_FILES
${SOURCE_FILES}
 ${CMAKE_CURRENT_SOURCE_DIR}/selectupgrademethoddlg.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/sendupgradefiledialog.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/sysupgradedialog.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/sysupgradewizard.cpp

 PARENT_SCOPE
)

set(UI_FILES
${UI_FILES}
 ${CMAKE_CURRENT_SOURCE_DIR}/selectupgrademethoddlg.ui
 ${CMAKE_CURRENT_SOURCE_DIR}/sendupgradefiledialog.ui
 ${CMAKE_CURRENT_SOURCE_DIR}/sysupgradedialog.ui
 ${CMAKE_CURRENT_SOURCE_DIR}/PARENT_SCOPE
)