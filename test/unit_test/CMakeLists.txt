cmake_minimum_required(VERSION 3.19)

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

message("Compiler Version: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}") 

# Include Boost as an imported target: 1.73 important for compilation with gcc 10 and C++20
find_package(Boost 1.79 REQUIRED COMPONENTS filesystem serialization)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS}) # Find Boost headers only
ENDIF()

# Global approach Gdal
find_package(LIBKML)
message(STATUS "LIBKML_FOUND: ${LIBKML_FOUND}")

find_package(GDAL REQUIRED)

# range-v3
find_package(range-v3)

# mp-units
find_package(mp-units CONFIG REQUIRED)

# Copy the data required
file(COPY ${PROJECT_SOURCE_DIR}/test/data DESTINATION ${CMAKE_BINARY_DIR}/test)

### UNIT TEST
file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} unit_test/*.cpp)
foreach(testSrc ${TEST_SRCS})
  # Extract the filename without an extension (NAME_WE)
  get_filename_component(testName ${testSrc} NAME_WE)
  # Add compile target
  add_executable(${testName} ${testSrc})
  # Require the standard
  target_compile_features(${testName} PRIVATE cxx_std_23)
  # Ignore warnings about subtle ABI change
  target_compile_options(${testName} PUBLIC "-Wno-psabi")
  # Link to targets and dependencies
  target_link_libraries(${testName}
  PRIVATE
    quetzal::quetzal
    Boost::boost
    GDAL::GDAL
    range-v3::range-v3
    mp-units::mp-units
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
  )  

  # Specifies include directories to use when compiling the target
  target_include_directories(
    ${testName} PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
  # Add it to test execution
  add_test(
    NAME ${testName}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
    COMMAND ${testName} )
endforeach(testSrc)