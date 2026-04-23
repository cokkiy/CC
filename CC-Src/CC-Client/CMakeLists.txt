cmake_minimum_required(VERSION 3.14)
project(CC_Client VERSION 3.5.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(Qt5 COMPONENTS Widgets Core Gui Network PrintSupport Concurrent Svg DataVisualization REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

define_property(CACHED_VARIABLE PROPERTY QWT_HOME BRIEF_DOCS "Qwt library home" FULL_DOCS "Qwt library home")

#add_link_options("/NODEFAULTLIB:library")
#add_subdirectory(qwt-6.2.0)
add_subdirectory(cpuplot)
add_subdirectory(src)
