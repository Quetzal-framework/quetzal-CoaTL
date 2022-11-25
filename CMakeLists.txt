cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

###########################
#    PROJECT DESCRIPTION
###########################

project(
  "quetzal"
  LANGUAGES CXX
  VERSION "0.1"
  DESCRIPTION "A header only c++ library for coalescence simulation."
  HOMEPAGE_URL "https://github.com/becheler/quetzal"
)

#############################
#    CONFIGURATION
#############################

# Use modern C++ with support for concepts and mp-units
set(CMAKE_CXX_STANDARD 20)
# Prevent use of non-portable compiler extensions
set(CMAKE_CXX_EXTENSIONS OFF)
# This makes C++20 a requirement and prevents a "decay" to C++98 when the compiler does not support C++20.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# We default to Release build type if option -D CMAKE_BUILD_TYPE not provided
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

######################################
#     CONAN dependencies management: using conan.tools.cmake
######################################

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Avoids Could not find a package configuration file provided by "mp-units"
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

# Tell find_package() to first search using Config mode before falling back to Module mode (for conan)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

#find_package(mp-units CONFIG REQUIRED)

# Include Boost as an imported target: 1.73 important for compilation with gcc 10 and C++20
find_package(Boost 1.79 REQUIRED COMPONENTS unit_test_framework filesystem serialization)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS}) # Find Boost headers only
ENDIF()

# Global approach Gdal
find_package(GDAL REQUIRED)

#########################################
#     Quetzal as a header only library
#########################################

message(STATUS "Project will be installed to ${CMAKE_INSTALL_PREFIX}")

# Creates an Interface Library: target does not compile sources
add_library(${PROJECT_NAME} INTERFACE )

# Add an alias so the project can be used with add_subdirectory
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Provides install directory variables as defined by the GNU Coding Standards.
include(GNUInstallDirs)

# Adding the install interface generator expression makes sure that the include
# files are installed to the proper location (provided by GNUInstallDirs)
target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                          )

# If we have compiler requirements for this library, list them here
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

####################################
#      Testing & Documentation
####################################

enable_testing()

add_subdirectory(test)

add_subdirectory(docs)

###########################
# INSTALLATION
###########################

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
