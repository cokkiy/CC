cmake_minimum_required(VERSION 3.10)
project(CC_Client VERSION 3.5.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

FIND_PACKAGE(Qt5 COMPONENTS Widgets Core Gui REQUIRED)
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
)
add_subdirectory("Slice")
# 生成可执行文件，需添加RCC_FILES、WRAP_FILES
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${RCC_FILES}  ${UI_FILES})

# 添加Qt5依赖项
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui)