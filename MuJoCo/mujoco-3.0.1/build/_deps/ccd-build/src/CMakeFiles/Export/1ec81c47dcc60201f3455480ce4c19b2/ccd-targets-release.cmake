#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ccd" for configuration "Release"
set_property(TARGET ccd APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ccd PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/ccd.lib"
  )

list(APPEND _cmake_import_check_targets ccd )
list(APPEND _cmake_import_check_files_for_ccd "${_IMPORT_PREFIX}/lib/ccd.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
