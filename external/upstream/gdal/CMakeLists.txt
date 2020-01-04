find_package(GDAL ${GDAL_MINIMUM_REQUIRED})

if(GDAL_FOUND)
  message(STATUS "Found GDAL version ${GDAL_VERSION}")
  add_library(gdal_external INTERFACE)
else()
  message(STATUS "GDAL ${GDAL_MINIMUM_REQUIRED} could not be located, Building GDAL 2.2 instead.")

  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(APPLE)
      set(_toolset "darwin")
    else()
      set(_toolset "gcc")
    endif()
  elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(_toolset "clang")
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    if(APPLE)
      set(_toolset "intel-darwin")
    else()
      set(_toolset "intel-linux")
    endif()
  endif()

  include(ExternalProject)
  ExternalProject_Add(gdal_external
    URL
      https://sourceforge.net/projects/boost/files/boost/1.61.0/boost_1_61_0.zip
    URL_HASH
      SHA256=02d420e6908016d4ac74dfc712eec7d9616a7fc0da78b0a1b5b937536b2e01e8
    DOWNLOAD_NO_PROGRESS
      1
    UPDATE_COMMAND
      ""
    CONFIGURE_COMMAND
      <SOURCE_DIR>/bootstrap.sh
      --with-toolset=${_toolset}
      --prefix=${STAGED_INSTALL_PREFIX}/boost
      ${_bootstrap_select_libraries}
    BUILD_COMMAND
      <SOURCE_DIR>/b2 -q
           link=shared
           threading=multi
           variant=release
           toolset=${_toolset}
           ${_b2_select_libraries}
    LOG_BUILD
      1
    BUILD_IN_SOURCE
      1
    INSTALL_COMMAND
      <SOURCE_DIR>/b2 -q install
           link=shared
           threading=multi
           variant=release
           toolset=${_toolset}
           ${_b2_select_libraries}
    LOG_INSTALL
      1
    BUILD_BYPRODUCTS
      "${_build_byproducts}"
    )

  set(
    BOOST_ROOT ${STAGED_INSTALL_PREFIX}/boost
    CACHE PATH "Path to internally built Boost installation root"
    FORCE
    )
  set(
    BOOST_INCLUDEDIR ${BOOST_ROOT}/include
    CACHE PATH "Path to internally built Boost include directories"
    FORCE
    )
  set(
    BOOST_LIBRARYDIR ${BOOST_ROOT}/lib
    CACHE PATH "Path to internally built Boost library directories"
    FORCE
    )

  # Unset internal variables
  unset(_toolset)
  unset(_b2_needed_components)
  unset(_build_byproducts)
  unset(_b2_select_libraries)
  unset(_boostrap_select_libraries)
endif()
