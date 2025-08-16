cmake_minimum_required(VERSION 3.14)

project("ncmdump")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(WITH_ZLIB OFF)
set(BUILD_TESTING OFF)
add_subdirectory(taglib)

FILE(GLOB COMMON_HEADERS src/include/*.h)
FILE(GLOB COMMON_SOURCES src/ncmcrypt.cpp src/utils/*.cpp)
FILE(GLOB EXECUTABLE_SOURCES src/main.cpp)
FILE(GLOB LIBRARY_HEADERS src/lib/libncmdump.h)
FILE(GLOB LIBRARY_SOURCES src/lib/*.cpp)
FILE(GLOB WIN_SOURCES src/platform/win32_init.cpp)

add_library(ncmdump_lib SHARED
  ${COMMON_HEADERS}
  ${COMMON_SOURCES}
  ${LIBRARY_HEADERS}
  ${LIBRARY_SOURCES})

set_target_properties(ncmdump_lib PROPERTIES OUTPUT_NAME "ncmdump")

target_include_directories(ncmdump_lib PRIVATE src/include)

target_link_libraries(ncmdump_lib tag)
target_include_directories(ncmdump_lib PRIVATE taglib)
target_include_directories(ncmdump_lib PRIVATE taglib/taglib)
target_include_directories(ncmdump_lib PRIVATE taglib/taglib/toolkit)
target_include_directories(ncmdump_lib PRIVATE taglib/taglib/mpeg/id3v2)

include(GNUInstallDirs)