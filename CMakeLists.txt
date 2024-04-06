cmake_minimum_required(VERSION 3.14)

project(ncmdump LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(WITH_ZLIB OFF)
set(BUILD_TESTING OFF)
add_subdirectory(taglib)

if(MSVC)
  add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:utf-8>")
endif()

FILE(GLOB COMMON_HEADERS src/include/*.h)
FILE(GLOB COMMON_SOURCES src/ncmcrypt.cpp src/utils/*.cpp)
FILE(GLOB EXECUTABLE_SOURCES src/main.cpp)
FILE(GLOB LIBRARY_HEADERS src/lib/libncmdump.h)
FILE(GLOB LIBRARY_SOURCES src/lib/*.cpp)
FILE(GLOB WIN_SOURCES src/platform/win32_init.cpp)

if(WIN32)
  add_executable(ncmdump_exec
    ${COMMON_HEADERS}
    ${COMMON_SOURCES}
    ${EXECUTABLE_SOURCES}
    ${WIN_SOURCES}
  )
  add_library(ncmdump_lib SHARED
    ${COMMON_HEADERS}
    ${COMMON_SOURCES}
    ${LIBRARY_HEADERS}
    ${LIBRARY_SOURCES}
    ${WIN_SOURCES}
  )

  set_target_properties(ncmdump_lib PROPERTIES OUTPUT_NAME "libncmdump")

  target_include_directories(ncmdump_lib PRIVATE src/include)

  target_link_libraries(ncmdump_lib tag)
  target_include_directories(ncmdump_lib PRIVATE taglib)
  target_include_directories(ncmdump_lib PRIVATE taglib/taglib)
  target_include_directories(ncmdump_lib PRIVATE taglib/taglib/toolkit)
  target_include_directories(ncmdump_lib PRIVATE taglib/taglib/mpeg/id3v2)

  if(CMAKE_COMPILER_IS_GNUCXX)
    # static link when using MinGW
    target_link_options(ncmdump_exec PRIVATE -static)
    target_link_options(ncmdump_lib PRIVATE -static)
  endif()
else()
  add_executable(ncmdump_exec
    ${COMMON_HEADERS}
    ${COMMON_SOURCES}
    ${EXECUTABLE_SOURCES}
  )
endif()

# all executable target
set_target_properties(ncmdump_exec PROPERTIES OUTPUT_NAME "ncmdump")
target_include_directories(ncmdump_exec PRIVATE src/include)

target_link_libraries(ncmdump_exec tag)
target_include_directories(ncmdump_exec PRIVATE taglib)
target_include_directories(ncmdump_exec PRIVATE taglib/taglib)
target_include_directories(ncmdump_exec PRIVATE taglib/taglib/toolkit)
target_include_directories(ncmdump_exec PRIVATE taglib/taglib/mpeg/id3v2)

include(GNUInstallDirs)

if(WIN32)
  install(TARGETS ncmdump_exec ncmdump_lib
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
else()
  install(TARGETS ncmdump_exec
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
endif()
