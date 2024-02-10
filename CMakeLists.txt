cmake_minimum_required(VERSION 3.14)

project(ncmdump LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(taglib)

FILE(GLOB HEADERS ./*.h)
FILE(GLOB SOURCES ./*.cpp)

#Shared Lib
add_library(ncmdumplib SHARED
    ${HEADERS}
    ${SOURCES}
)
target_link_libraries(ncmdumplib tag)
target_include_directories(ncmdumplib PRIVATE taglib)
target_include_directories(ncmdumplib PRIVATE taglib/taglib)
target_include_directories(ncmdumplib PRIVATE taglib/taglib/toolkit)
target_include_directories(ncmdumplib PRIVATE taglib/taglib/mpeg/id3v2)

target_compile_definitions(ncmdumplib PRIVATE NCMDUMP_LIBRARY)

#Executable File
add_executable(ncmdump
    main.cpp
)
target_link_libraries(ncmdump ncmdumplib)

include(GNUInstallDirs)
install(TARGETS ncmdumplib
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
