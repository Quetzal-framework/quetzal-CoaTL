cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

### UNIT TEST

# Keep test files in a separate source directory called test
file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} unit_test/*.cpp)

#Run through each source
foreach(testSrc ${TEST_SRCS})
        #Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${testSrc} NAME_WE)

        #Add compile target
        add_executable(${testName} ${testSrc} ${version_file})
        target_compile_options(${testName} PRIVATE "-fconcepts")

        #link to Boost libraries AND your targets and dependencies
        target_link_libraries(${testName}
                              boost::boost       # global target
                              ${Boost_LIBRARIES} # Boost component libraries to be linked.
                              Boost::headers     # Target for header-only dependencies.
                              # Boost::unit_test_framework
                              # Boost::filesystem
                              # Boost::serialization
                              GDAL::GDAL
                              mp-units::mp-units
                              mp-units::core-io mp-units::si
                              )

        #I like to move testing binaries into a testBin directory
        set_target_properties(${testName} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/build/testBin)

        target_include_directories(
          ${testName} PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

        #Finally add it to test execution -
        #Notice the WORKING_DIRECTORY and COMMAND
        add_test(NAME ${testName}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/testBin
                 COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build/testBin/${testName} )
endforeach(testSrc)

### INTEGRATION TESTS

### MODEL 1

add_executable(model_1 integration_test/model_1.cpp)

target_link_libraries(model_1 LINK_PUBLIC GDAL::GDAL ${GDAL_LIBRARY} )

target_include_directories(model_1
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
add_test(SpatialExpansionMassBased model_1 ${CMAKE_CURRENT_SOURCE_DIR}/data/europe_temp.tif)

### MODEL 2

add_executable(model_2 integration_test/model_2.cpp)

target_link_libraries(model_2 LINK_PUBLIC GDAL::GDAL ${GDAL_LIBRARY} )

target_include_directories(model_2
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
add_test(SpatialExpansionIndividualBased model_2 ${CMAKE_CURRENT_SOURCE_DIR}/data/europe_temp.tif)


### MODEL 3

add_executable(model_3 integration_test/model_3.cpp)

target_link_libraries(model_3 LINK_PUBLIC
                              boost::boost       # globalCMake's target
                              ${Boost_LIBRARIES} # Boost component libraries to be linked.
                              Boost::headers     # Target for header-only dependencies.
                              Boost::unit_test_framework
                              Boost::serialization
                              GDAL::GDAL
                              ${GDAL_LIBRARY}
                            )

target_include_directories(model_3
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
add_test(SpatialExpansionMassBasedSerialized model_3 ${CMAKE_CURRENT_SOURCE_DIR}/data/europe_temp.tif)

### TUTORIAL TESTS
#
# add_executable(tuto_1 tutorials_test/tuto_1.cpp)
# target_link_libraries(tuto_1 LINK_PUBLIC ${GDAL_LIBRARY} )
# target_include_directories(tuto_1
#   PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
#   $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
# add_test(TwoDemesIndividualBased tuto_1)


# Keep test files in a separate source directory called test
file(GLOB TUTO_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tutorials_test/*.cpp)

# Run through each source
foreach(testSrc ${TUTO_SRCS})

        # Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${testSrc} NAME_WE)

        # Add compile target
        add_executable(${testName} ${testSrc})

        # Link to Boost libraries AND other targets and dependencies
        target_link_libraries(${testName}
                              boost::boost       # global target
                              ${Boost_LIBRARIES} # component libraries to be linked.
                              Boost::headers     # target for header-only dependencies.
                              Boost::unit_test_framework
                              GDAL::GDAL         # global target
                              ${GDAL_LIBRARY}  # GDAL component libraries to be linked.
                              mp-units::mp-units
                              )

        # I like to move testing binaries into a testBin directory
        set_target_properties(${testName} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/build/testBin)

        # Specifies include directories to use when compiling a given target
        target_include_directories(
          ${testName} PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

        #Finally add it to test execution: notice the WORKING_DIRECTORY and COMMAND
        add_test(NAME ${testName}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/testBin
                 COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build/testBin/${testName})

endforeach(testSrc)
