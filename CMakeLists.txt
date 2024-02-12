cmake_minimum_required(VERSION 3.14)

project(ncmdump LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(WITH_ZLIB OFF)
set(BUILD_TESTING OFF)
add_subdirectory(taglib)

FILE(GLOB HEADERS ./*.h)
FILE(GLOB SOURCES ./*.cpp)
add_executable(ncmdump
  ${HEADERS}
  ${SOURCES}
)
target_link_libraries(ncmdump tag)
target_include_directories(ncmdump PRIVATE taglib)
target_include_directories(ncmdump PRIVATE taglib/taglib)
target_include_directories(ncmdump PRIVATE taglib/taglib/toolkit)
target_include_directories(ncmdump PRIVATE taglib/taglib/mpeg/id3v2)

if(WIN32)
  if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_options(ncmdump PRIVATE -O2 -municode)
  endif()
endif()

include(GNUInstallDirs)
install(TARGETS ncmdump
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
