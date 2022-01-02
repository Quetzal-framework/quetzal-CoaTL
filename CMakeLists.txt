cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

#
# VERSIONING
#

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

include(GetGitRevisionDescription)
git_describe(VERSION --tags --dirty=-d)

#parse the version information into pieces.
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")
set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/version.cpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
                
set(version_file "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

#Add the version_file to the project build or it won't compile.
#add_executable(${PROJECT_NAME} ${source_files} ${ui_files} ${version_file})

#
# PROJECT DESCRIPTION
#
project(
  "quetzal"
  LANGUAGES CXX
  VERSION ${VERSION_SHORT}
  DESCRIPTION "A header only c++ library for spatially explicit coalescence simulation."
  HOMEPAGE_URL "https://github.com/becheler/quetzal")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Project will be installed to ${CMAKE_INSTALL_PREFIX}")

include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE )
# add alias so the project can be used with add_subdirectory
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Include Boost as an imported target
find_package(Boost 1.61 REQUIRED COMPONENTS unit_test_framework filesystem serialization)

# Include Gdal
find_package(GDAL REQUIRED)
IF (GDAL_FOUND)
    INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})
ENDIF()

# Adding the install interface generator expression makes sure that the include
# files are installed to the proper location (provided by GNUInstallDirs)
target_include_directories(${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# If we have compiler requirements for this library, list them here
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

#
# TESTING
#
enable_testing()
add_subdirectory(test)

#
# DOCUMENTATION
#
add_subdirectory(docs)

#
# INSTALLATION
#
# 'make install' to the correct locations provided by GNUInstallDirs.
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# This is for Windows
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# The header files are copied to the installation folder
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/quetzal DESTINATION include)
