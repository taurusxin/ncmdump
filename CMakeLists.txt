cmake_minimum_required(VERSION 3.14)

project(ncmdump LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZLIB)
find_package(TagLib REQUIRED)

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

  if(MSVC)
    # 强制所有配置（Debug/Release 等）使用静态 C 运行时库
    foreach(flag_var
      CMAKE_CXX_FLAGS
      CMAKE_C_FLAGS
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_RELEASE
    )
      if(${flag_var})
        string(REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif()
    endforeach()
  endif()

  target_include_directories(ncmdump_lib PRIVATE src/include)

  target_compile_definitions(ncmdump_exec PRIVATE TAGLIB_STATIC)

  set_target_properties(ncmdump_lib PROPERTIES OUTPUT_NAME "libncmdump")

  target_link_libraries(ncmdump_lib TagLib::tag)

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

# link to taglib
target_link_libraries(ncmdump_exec TagLib::tag)

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
