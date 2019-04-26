cmake_minimum_required(VERSION 2.8)

project(quetzal)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -pedantic-errors -Wextra -DNDEBUG)

FIND_PACKAGE(Boost COMPONENTS unit_test_framework system filesystem REQUIRED)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS(-DBOOST_UBLAS_NDEBUG)
ENDIF()

FIND_PACKAGE(GDAL)
IF (GDAL_FOUND)
    INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})
ENDIF()

include_directories( ./ )

### TESTS

enable_testing()

## INTEGRATION

add_executable(model_1 test/integration_test/spatial_expansion/model_1/main.cpp)
TARGET_LINK_LIBRARIES(model_1 LINK_PUBLIC ${GDAL_LIBRARY} )
add_test(SpatialExpansion model_1 ${CMAKE_CURRENT_SOURCE_DIR}/test/data/europe_temp.tif)

## UNIT

# Keep test files in a separate source directory called test
file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/unit_test/*.cpp)

#Run through each source
foreach(testSrc ${TEST_SRCS})
        #Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${testSrc} NAME_WE)

        #Add compile target
        add_executable(${testName} ${testSrc})

        #link to Boost libraries AND your targets and dependencies
        target_link_libraries(${testName}
                                ${Boost_LIBRARIES}
                                ${Boost_FILESYSTEM_LIBRARY}
                                ${Boost_SYSTEM_LIBRARY}
                                ${GDAL_LIBRARY})

        #I like to move testing binaries into a testBin directory
        set_target_properties(${testName} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/build/testBin)

        #Finally add it to test execution -
        #Notice the WORKING_DIRECTORY and COMMAND
        add_test(NAME ${testName}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/testBin
                 COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build/testBin/${testName} )
endforeach(testSrc)
