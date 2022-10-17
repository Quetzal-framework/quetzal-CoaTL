cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)

### UNIT TEST
file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} unit_test/*.cpp)
foreach(testSrc ${TEST_SRCS})
  # Extract the filename without an extension (NAME_WE)
  get_filename_component(testName ${testSrc} NAME_WE)
  # Add compile target
  add_executable(${testName} ${testSrc})
  # Link to targets and dependencies
  target_link_libraries(${testName} boost::boost GDAL::GDAL)
  # Specifies include directories to use when compiling the target
  target_include_directories(
    ${testName} PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
  # Add it to test execution
  add_test(
    NAME ${testName}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMAND ${testName} )
endforeach(testSrc)

### INTEGRATION TESTS
file(GLOB INTEGRATION_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} integration_test/*.cpp)
foreach(testSrc ${INTEGRATION_SRCS})
  # Extract the filename without an extension (NAME_WE)
  get_filename_component(testName ${testSrc} NAME_WE)
  # Add compile target
  add_executable(${testName} ${testSrc})
  # Link to targets and dependencies
  target_link_libraries(${testName} boost::boost GDAL::GDAL)
  # Specifies include directories to use when compiling the target
  target_include_directories(
    ${testName} PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
  # Add it to test execution
  add_test(
    NAME ${testName}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMAND ${testName} ${CMAKE_CURRENT_SOURCE_DIR}/data/europe_temp.tif)
endforeach(testSrc)

### TUTORIAL TESTS
file(GLOB TUTO_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tutorials_test/*.cpp)
foreach(testSrc ${TUTO_SRCS})
  # Extract the filename without an extension (NAME_WE)
  get_filename_component(testName ${testSrc} NAME_WE)
  # Add compile target
  add_executable(${testName} ${testSrc})
  # Link to Boost libraries AND other targets and dependencies
  target_link_libraries(${testName} boost::boost GDAL::GDAL)
  # Specifies include directories to use when compiling a given target
  target_include_directories(
    ${testName} PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
  # Add it to test execution
  add_test(
    NAME ${testName}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMAND ${testName})
endforeach(testSrc)
