# FindTaglib.cmake
# Try to find TagLib library and headers
# Once done, it will define:
#   Taglib_FOUND
#   Taglib_INCLUDE_DIRS
#   Taglib_LIBRARIES
#   Taglib::Taglib (imported target)

find_path(Taglib_INCLUDE_DIR tag.h
  PATH_SUFFIXES taglib
  PATHS /usr/include /usr/local/include
)

find_library(Taglib_LIBRARY NAMES tag taglib
  PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

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
endif()
