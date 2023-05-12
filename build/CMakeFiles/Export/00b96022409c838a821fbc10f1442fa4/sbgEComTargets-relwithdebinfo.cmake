#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "sbg::sbgECom" for configuration "RelWithDebInfo"
set_property(TARGET sbg::sbgECom APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(sbg::sbgECom PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/sbgECom.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/sbgECom.dll"
  )

list(APPEND _cmake_import_check_targets sbg::sbgECom )
list(APPEND _cmake_import_check_files_for_sbg::sbgECom "${_IMPORT_PREFIX}/lib/sbgECom.lib" "${_IMPORT_PREFIX}/bin/sbgECom.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
