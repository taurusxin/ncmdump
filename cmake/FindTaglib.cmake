# FindTaglib.cmake
#
# Cross-platform TagLib finder.
# Supports:
#   - Ubuntu/Debian (libtag1v5-dev)
#   - macOS (Homebrew)
#   - Windows (vcpkg/manual)
#
# Defines:
#   Taglib_FOUND
#   Taglib_INCLUDE_DIRS
#   Taglib_LIBRARIES
#   Taglib::Taglib (imported target)

# --- First: if official Config file exists (v2.x), use it directly ---
find_package(Taglib CONFIG QUIET)
if(Taglib_FOUND)
  message(STATUS "Using system-provided Taglib (with CMake config).")
  return()
endif()

# --- Manual search fallback (for TagLib v1.x without config) ---
message(STATUS "No Taglib CMake config found, falling back to manual search...")

if(WIN32)
  # --- Windows ---
  find_path(Taglib_INCLUDE_DIR tag.h
    PATH_SUFFIXES taglib
    PATHS
      "C:/Program Files/taglib/include"
      "C:/Program Files (x86)/taglib/include"
      "${CMAKE_SOURCE_DIR}/third_party/taglib/include"
  )
  find_library(Taglib_LIBRARY NAMES tag taglib
    PATHS
      "C:/Program Files/taglib/lib"
      "C:/Program Files (x86)/taglib/lib"
      "${CMAKE_SOURCE_DIR}/third_party/taglib/lib"
  )

elseif(APPLE)
  # --- macOS ---
  find_path(Taglib_INCLUDE_DIR tag.h
    PATH_SUFFIXES taglib
    PATHS
      /usr/local/include
      /opt/homebrew/include
  )
  find_library(Taglib_LIBRARY NAMES tag taglib
    PATHS
      /usr/local/lib
      /opt/homebrew/lib
  )

else()
  # --- Linux / Unix ---
  find_path(Taglib_INCLUDE_DIR tag.h
    PATH_SUFFIXES taglib
    PATHS
      /usr/include
      /usr/local/include
  )
  find_library(Taglib_LIBRARY NAMES tag taglib
    PATHS
      /usr/lib /usr/lib64
      /usr/local/lib /usr/local/lib64
      /usr/lib/x86_64-linux-gnu
  )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Taglib DEFAULT_MSG
  Taglib_LIBRARY Taglib_INCLUDE_DIR
)

if(Taglib_FOUND)
  set(Taglib_INCLUDE_DIRS ${Taglib_INCLUDE_DIR})
  set(Taglib_LIBRARIES ${Taglib_LIBRARY})

  if(NOT TARGET Taglib::Taglib)
    add_library(Taglib::Taglib UNKNOWN IMPORTED)
    set_target_properties(Taglib::Taglib PROPERTIES
      IMPORTED_LOCATION ${Taglib_LIBRARY}
      INTERFACE_INCLUDE_DIRECTORIES ${Taglib_INCLUDE_DIR}
    )
  endif()
else()
  message(WARNING "Taglib not found. Please install libtag1v5-dev (Linux), \
brew install taglib (macOS), or add manual path (Windows).")
endif()
