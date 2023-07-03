#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "sbg::sbgECom" for configuration "Release"
set_property(TARGET sbg::sbgECom APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(sbg::sbgECom PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsbgECom.so"
  IMPORTED_SONAME_RELEASE "libsbgECom.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS sbg::sbgECom )
list(APPEND _IMPORT_CHECK_FILES_FOR_sbg::sbgECom "${_IMPORT_PREFIX}/lib/libsbgECom.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
