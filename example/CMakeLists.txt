# requires cxx_std_20 new in CMake 3.12
cmake_minimum_required(VERSION 3.12)

# Tell find_package() to first search using Config mode before falling back to Module mode (for conan)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

# Global approach Gdal
find_package(GDAL REQUIRED)

# Include Boost as an imported target: 1.73 important for compilation with gcc 10 and C++20
find_package(Boost 1.79 REQUIRED COMPONENTS unit_test_framework filesystem serialization)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS}) # Find Boost headers only
ENDIF()

# Create director to store the examples output (for future doc)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/example/output)

file(GLOB SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach(testSrc ${SRCS})
  # Extract the filename without an extension (NAME_WE)
  get_filename_component(testName ${testSrc} NAME_WE)
  # Add compile target
  add_executable(${testName} ${testSrc})
  # Require the standard
  target_compile_features(${testName} PUBLIC cxx_std_20)
  # Link to Boost libraries AND other targets and dependencies
  target_link_libraries(${testName} quetzal::quetzal boost::boost GDAL::GDAL)
  # Specifies include directories to use when compiling a given target
  target_include_directories(
    ${testName} PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

  # Add it to test execution
  add_test(
    NAME ${testName}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/example
  COMMAND sh -c "$<TARGET_FILE:${testName}> > output/${testName}.txt" )
endforeach(testSrc)