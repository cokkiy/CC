
set(HEADER_FILES
${HEADER_FILES}
${CMAKE_CURRENT_SOURCE_DIR}/packetgraph.h
${CMAKE_CURRENT_SOURCE_DIR}/variantbardatamapping.h
${CMAKE_CURRENT_SOURCE_DIR}/variantbardataproxy.h
${CMAKE_CURRENT_SOURCE_DIR}/variantdataset.h
${CMAKE_CURRENT_SOURCE_DIR}/fakecapturedata.h
${CMAKE_CURRENT_SOURCE_DIR}/captureprogressdlg.h
${CMAKE_CURRENT_SOURCE_DIR}/packetanalyzewindow.h
PARENT_SCOPE
)

set(SOURCE_FILES
${SOURCE_FILES}
 ${CMAKE_CURRENT_SOURCE_DIR}/packetgraph.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/variantbardatamapping.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/variantbardataproxy.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/variantdataset.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/fakecapturedata.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/captureprogressdlg.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/packetanalyzewindow.cpp
 PARENT_SCOPE
)

set(UI_FILES
${UI_FILES}
 ${CMAKE_CURRENT_SOURCE_DIR}/captureprogressdlg.ui
 ${CMAKE_CURRENT_SOURCE_DIR}/packetanalyzewindow.ui
 PARENT_SCOPE
)

