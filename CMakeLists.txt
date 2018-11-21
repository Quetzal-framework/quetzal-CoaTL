cmake_minimum_required(VERSION 2.8)

project(quetzal)

add_compile_options(-std=c++17)

FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()

FIND_PACKAGE(GDAL)
IF (GDAL_FOUND)
    INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})
ENDIF()

include_directories( ./ )

# DOCUMENTATION
# add_executable(two_demes documentation/two_demes/main.cpp)

add_executable(expansion_1 documentation/code_examples/expansion/main_1.cpp)
TARGET_LINK_LIBRARIES(expansion_1 LINK_PUBLIC ${GDAL_LIBRARY} )


# creates the executables
# add_executable(itest test/integration_test/integration.cpp)
# add_executable(utest1 test/unit_test/simulator/simulator_test.cpp)
# add_executable(utest2 test/unit_test/demography/History_test.cpp)
add_executable(expressive_test test/unit_test/expressive/expressive_test.cpp)
add_executable(utils_test test/unit_test/utils/matrix_operation_test.cpp)

#TARGET_LINK_LIBRARIES( itest LINK_PUBLIC ${GDAL_LIBRARY} )
