set(HEADERS
   counterpiemarker.h
   cpustat.h
   plotpart.h
)

add_library(cpuplot STATIC
    ${HEADERS}
    counterpiemarker.cpp
)
target_link_libraries(cpuplot PRIVATE debug qwtd optimized qwt Qt5::Core Qt5::Gui Qt5::Widgets)
target_include_directories(cpuplot INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(cpuplot PRIVATE ${QWT_HOME}/include)
target_link_directories(cpuplot PRIVATE ${QWT_HOME}/lib)
target_link_options(cpuplot PRIVATE "/MTd")