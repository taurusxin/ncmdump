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

FILE(GLOB HEADERS ./src/include/*.h)
FILE(GLOB COMMON_SOURCES ./src/ncmcrypt.cpp ./src/utils/*.cpp)
FILE(GLOB EXECUTABLE_SOURCES ./src/main.cpp)
FILE(GLOB LIBRARY_SOURCES ./src/lib/libncmdump.cpp)
FILE(GLOB WIN_EXEC_SOURCES ./src/platform/win32_init.cpp)

if(WIN32)
  add_executable(ncmdump
    ${HEADERS}
    ${COMMON_SOURCES}
    ${EXECUTABLE_SOURCES}
    ${WIN_EXEC_SOURCES}
  )
  add_library(libncmdump SHARED
    ${HEADERS}
    ${COMMON_SOURCES}
    ${LIBRARY_SOURCES}
  )

  target_include_directories(libncmdump PRIVATE src/include)

  target_link_libraries(libncmdump tag)
  target_include_directories(libncmdump PRIVATE taglib)
  target_include_directories(libncmdump PRIVATE taglib/taglib)
  target_include_directories(libncmdump PRIVATE taglib/taglib/toolkit)
  target_include_directories(libncmdump PRIVATE taglib/taglib/mpeg/id3v2)

  if(CMAKE_COMPILER_IS_GNUCXX)
    # static link when using MinGW
    target_link_options(ncmdump PRIVATE -static)
    target_link_options(libncmdump PRIVATE -static)
  endif()
else()
  add_executable(ncmdump
    ${HEADERS}
    ${COMMON_SOURCES}
    ${EXECUTABLE_SOURCES}
  )
endif()

include(GNUInstallDirs)
install(TARGETS ncmdump
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
