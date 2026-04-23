
set(HEADER_FILES
aboutdialog.h
batchcapturedialog.h
batchcaputrethread.h
CompareOperation.h
defaultappprocdialog.h
detaildialog.h
editstationdialog.h
executcounting.h
filebrowserdialog.h
FileVersion.h
filterbuilddialog.h
FilterOperations.h
floatingmenu.h
mainwindow.h
monitor.h
NetworkInterface.h
option.h
processdialog.h
ProgressBarDelegate.h
qpagenumdialog.h
qtdispatcher.h
recvfiledialog.h
recvfilethread.h
screenimagedialog.h
sendfiledialog.h
sendfilethread.h
setintervaldialog.h
stationbar.h
stationdetaildialog.h
StationInfo.h
StationList.h
stationmanager.h
StationStateReceiver.h
stationtablemodel.h
updatemanager.h
upgradepreparethread.h
upgradestationservicedlg
viewlogdialog.h
weatherimageoptiondlg.h
xattribute.h
xdocument.h
xelement.h
netconfigdlg.h
autonetcfgdialog.h
createvnidialog.h
)

set(SOURCE_FILES
 aboutdialog.cpp
 batchcapturedialog.cpp
 batchcaputrethread.cpp
 CompareOperation.cpp
 defaultappprocdialog.cpp
 detaildialog.cpp
 editstationdialog.cpp
 executcounting.cpp
 filebrowserdialog.cpp
 FileVersion.cpp
 filterbuilddialog.cpp
 FilterOperations.cpp
 floatingmenu.cpp
 main.cpp
 mainwindow.cpp
 monitor.cpp
 NetworkInterface.cpp
 option.cpp
 processdialog.cpp
 ProgressBarDelegate.cpp
 qpagenumdialog.cpp
 qtdispatcher.cpp
 recvfiledialog.cpp
 recvfilethread.cpp
 screenimagedialog.cpp
 sendfiledialog.cpp
 sendfilethread.cpp
 setintervaldialog.cpp
 stationbar.cpp
 stationdetaildialog.cpp
 StationInfo.cpp
 StationList.cpp
 stationmanager.cpp
 StationStateReceiver.cpp
 stationtablemodel.cpp
 updatemanager.cpp
 upgradepreparethread.cpp
 upgradestationservicedlg.cpp
 viewlogdialog.cpp
 weatherimageoptiondlg.cpp
 xattribute.cpp
 xdocument.cpp
 xelement.cpp
 netconfigdlg.cpp
 autonetcfgdialog.cpp
 createvnidialog.cpp
)

set(RCC_FILES
  cc-client.qrc
)

set(UI_FILES
aboutdialog.ui
batchcapturedialog.ui
defaultappprocdialog.ui
detaildialog.ui
editstationdialog.ui
filebrowserdialog.ui
filterBuildDialog.ui
floatingmenu.ui
mainwindow.ui
processdialog.ui
qpagenumdialog.ui
recvfiledialog.ui
screenimagedialog.ui
sendfiledialog.ui
setintervaldialog.ui
stationdetaildialog.ui
upgradestationservicedlg.ui
viewlogdialog.ui
weatherimageoptiondlg.ui
netconfigdlg.ui
autonetcfgdialog.ui
createvnidialog.ui
)

# add sub directories for packetgraph
add_subdirectory(packetgraph)
add_subdirectory(sysupgrade)

find_package(Ice 3.6 COMPONENTS Ice IceUtil)
## compile ice file 
set(SLICE_FILES)
add_subdirectory("Slice")
include_directories(${Ice_INCLUDE_DIR})
set(COMPILED_SLICES)
set(OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

foreach(ice_file ${SLICE_FILES})
  message("slice compiling ${ice_file}...")
  string(REPLACE ".ice" ".h" header_file ${ice_file})
  string(REPLACE ".ice" ".cpp" cpp_file ${ice_file})
  add_custom_command(
  OUTPUT ${OUTDIR}/${header_file}  ${OUTDIR}/${cpp_file}
  COMMAND ${Ice_SLICE2CPP_EXECUTABLE} -I. --output-dir ${OUTDIR}  ${CMAKE_CURRENT_SOURCE_DIR}/Slice/${ice_file}
  BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/Slice/${ice_file}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Slice/${ice_file}
  )
  list(APPEND COMPILED_SLICES ${OUTDIR}/${header_file}  ${OUTDIR}/${cpp_file})
endforeach(ice_file )
# add ICE_CPP11_MAPPING define for C++ 11 mapping
add_compile_definitions(ICE_CPP11_MAPPING)


# 生成可执行文件，需添加RCC_FILES、WRAP_FILES
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${RCC_FILES}  ${UI_FILES} ${COMPILED_SLICES})


#translate
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(lrelease_EXECUTABLE lrelease HINTS "${_qt_bin_dir}")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${lrelease_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/cc-client_zh.ts -qm ${CMAKE_CURRENT_BINARY_DIR}/$<$<CONFIG:DEBUG>:Debug>$<$<CONFIG:RELEASE>:Release>/cc-client_zh.qm
  BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/cc-client_zh.ts
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cc-client_zh.ts
)

if($<CONFIG:DEBUG>:RELEASE>)
# copy qt.conf 
add_custom_command (TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/qt.conf $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
endif()

# copy config.server file
add_custom_command (TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.Server $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

target_include_directories(${PROJECT_NAME} PRIVATE ${OUTDIR})
#message("${Ice_Util_LIBRARY}" "${Ice_Util_LIBRARIES}" "${Ice_LIBRARIES}")
# 添加Qt5依赖项
target_link_libraries(${PROJECT_NAME} PRIVATE debug qwtd optimized qwt cpuplot Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Network Qt5::PrintSupport Qt5::Concurrent Qt5::Svg Qt5::DataVisualization ${Ice_LIBRARIES} )
target_include_directories(${PROJECT_NAME} PRIVATE ${QWT_HOME}/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${QWT_HOME}/lib)
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
