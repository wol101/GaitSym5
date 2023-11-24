#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mujoco::mujoco" for configuration "MinSizeRel"
set_property(TARGET mujoco::mujoco APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(mujoco::mujoco PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/mujoco.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/mujoco.dll"
  )

list(APPEND _cmake_import_check_targets mujoco::mujoco )
list(APPEND _cmake_import_check_files_for_mujoco::mujoco "${_IMPORT_PREFIX}/lib/mujoco.lib" "${_IMPORT_PREFIX}/bin/mujoco.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
