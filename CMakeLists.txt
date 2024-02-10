cmake_minimum_required(VERSION 3.14)

project(ncmdump LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_NAME ncmcrypt)
set(EXE_NAME ncmdump)

add_subdirectory(taglib)

FILE(GLOB HEADERS ./*.h)
FILE(GLOB SOURCES ./*.cpp)

#Lib
add_library(${LIB_NAME}
    ${HEADERS}
    ${SOURCES}
)
target_link_libraries(${LIB_NAME} tag)
target_include_directories(${LIB_NAME} PRIVATE taglib)
target_include_directories(${LIB_NAME} PRIVATE taglib/taglib)
target_include_directories(${LIB_NAME} PRIVATE taglib/taglib/toolkit)
target_include_directories(${LIB_NAME} PRIVATE taglib/taglib/mpeg/id3v2)

target_compile_definitions(${LIB_NAME} PRIVATE NCMDUMP_LIBRARY)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(${LIB_NAME} PRIVATE BUILD_SHARED_LIBS)
endif()

#Executable File
add_executable(${EXE_NAME}
    main.cpp
)
target_link_libraries(${EXE_NAME} ${LIB_NAME})
