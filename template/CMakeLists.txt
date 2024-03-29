# requires cxx_std_20 new in CMake 3.12
cmake_minimum_required(VERSION 3.12)

# Tell find_package() to first search using Config mode before falling back to Module mode (for conan)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

# Global approach Gdal
find_package(GDAL REQUIRED)

# range-v3
find_package(range-v3)

# mp-units
find_package(mp-units CONFIG REQUIRED)

# Include Boost as an imported target: 1.73 important for compilation with gcc 10 and C++20
find_package(Boost 1.79 REQUIRED COMPONENTS unit_test_framework filesystem serialization)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS}) # Find Boost headers only
ENDIF()

# Copy the data required by I/O examples
file(COPY data DESTINATION ${CMAKE_BINARY_DIR}/template)

# Create director to store the examples output (for future doc)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/template/output)

# Add compile target
add_executable(main main.cpp)
# Require the standard
target_compile_features(main PUBLIC cxx_std_20)
# Ignore warnings about subtle ABI change
target_compile_options(main PUBLIC "-Wno-psabi")
# Link to Boost libraries AND other targets and dependencies
target_link_libraries(main quetzal::quetzal boost::boost GDAL::GDAL range-v3::range-v3 mp-units::mp-units)
# Specifies include directories to use when compiling a given target
target_include_directories(
  main PRIVATE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
