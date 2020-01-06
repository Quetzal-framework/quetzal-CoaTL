find_package(GDAL ${GDAL_MINIMUM_REQUIRED})

if(GDAL_FOUND)
  message(STATUS "Found GDAL version ${GDAL_VERSION}")
  add_library(gdal_external INTERFACE)
endif()
