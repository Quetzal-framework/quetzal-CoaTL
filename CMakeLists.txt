cmake_minimum_required(VERSION 2.8)

project(quetzal)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -pedantic-errors -Wextra -DNDEBUG)

FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS(-DBOOST_UBLAS_NDEBUG)
ENDIF()

FIND_PACKAGE(GDAL)
IF (GDAL_FOUND)
    INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})
ENDIF()

include_directories( ./ )

### INTEGRATION TESTS

add_executable(model_1 test/integration_test/spatial_expansion/model_1/main.cpp)
TARGET_LINK_LIBRARIES(model_1 LINK_PUBLIC ${GDAL_LIBRARY} )

### UNIT TESTING

add_executable(expressive_test test/unit_test/expressive/expressive_test.cpp)
add_executable(utils_test test/unit_test/utils/matrix_operation_test.cpp)
