# requires cxx_std_20 new in CMake 3.12
cmake_minimum_required(VERSION 3.12)

# Tell find_package() to first search using Config mode before falling back to Module mode (for conan)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

# Global approach Gdal
find_package(LIBKML)
find_package(GDAL REQUIRED)

# range-v3
find_package(range-v3)

# mp-units
find_package(mp-units CONFIG REQUIRED)

# Include Boost as an imported target: 1.73 important for compilation with gcc 10 and C++20
find_package(Boost 1.79 REQUIRED COMPONENTS filesystem serialization)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS}) # Find Boost headers only
ENDIF()

# Copy the data required by I/O examples
file(COPY data DESTINATION ${CMAKE_BINARY_DIR}/example)

# Create director to store the examples output (for future doc)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/example/output)

file(GLOB SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach(testSrc ${SRCS})
  # Extract the filename without an extension (NAME_WE)
  get_filename_component(testName ${testSrc} NAME_WE)
  # Add compile target
  add_executable(${testName} ${testSrc})
  # Require the standard
  target_compile_features(${testName} PUBLIC cxx_std_23)
  # Ignore warnings about subtle ABI change
  target_compile_options(${testName} PUBLIC "-Wno-psabi")
  # Link to Boost libraries AND other targets and dependencies
  target_link_libraries(${testName} quetzal::quetzal Boost::boost GDAL::GDAL range-v3::range-v3 mp-units::mp-units)
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

  set_tests_properties(${testName} PROPERTIES LABELS "documentation example")

endforeach(testSrc)
