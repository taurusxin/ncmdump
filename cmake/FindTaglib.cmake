# --- Try official config first (Taglib 2.x) ---
find_package(Taglib CONFIG QUIET)
if(Taglib_FOUND)
  message(STATUS "Using system Taglib (CMake config found)")
  return()
endif()

# --- Try pkg-config (common on Linux) ---
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
  pkg_check_modules(TAGLIB taglib)
endif()

# --- Manual search fallback ---
if(TAGLIB_FOUND)
  set(Taglib_INCLUDE_DIRS ${TAGLIB_INCLUDE_DIRS})
  set(Taglib_LIBRARIES ${TAGLIB_LIBRARIES})
  message(STATUS "Using Taglib from pkg-config: ${Taglib_LIBRARIES}")
else()
  message(STATUS "Falling back to manual search for Taglib...")

  find_path(Taglib_INCLUDE_DIR tag.h
    PATH_SUFFIXES taglib
    PATHS
      /usr/include
      /usr/local/include
      /usr/include/taglib
  )

  find_library(Taglib_LIBRARY NAMES tag taglib
    PATHS
      /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64
      /usr/lib/x86_64-linux-gnu /usr/lib/aarch64-linux-gnu
  )

  set(Taglib_INCLUDE_DIRS ${Taglib_INCLUDE_DIR})
  set(Taglib_LIBRARIES ${Taglib_LIBRARY})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Taglib DEFAULT_MSG Taglib_LIBRARY Taglib_INCLUDE_DIR)

if(Taglib_FOUND)
  if(NOT TARGET Taglib::Taglib)
    add_library(Taglib::Taglib UNKNOWN IMPORTED)
    set_target_properties(Taglib::Taglib PROPERTIES
      IMPORTED_LOCATION ${Taglib_LIBRARY}
      INTERFACE_INCLUDE_DIRECTORIES ${Taglib_INCLUDE_DIR}
    )
  endif()
else()
  message(WARNING "Taglib not found! Try: sudo apt install libtag1v5-dev")
endif()
