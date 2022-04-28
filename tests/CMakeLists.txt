cmake_minimum_required(VERSION 3.8)

include_directories(${PROJECT_SOURCE_DIR}/src)

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIR})

set(PROJECT_LIBRARY_NAME "lib${PROJECT_NAME}")
add_library(${PROJECT_LIBRARY_NAME} STATIC ${${PROJECT_NAME}_FILES})

set(TEST_LIBRARIES ${CHECK_LIBRARIES} ${PROJECT_LIBRARY_NAME})
list(APPEND ${TEST_LIBRARIES} ${LIBXML2_LIBRARIES})

add_executable(test_fs "test_fs.c")
target_link_libraries(test_fs ${TEST_LIBRARIES})

add_test(NAME test_fs COMMAND test_fs WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Testing)
set_tests_properties(test_fs PROPERTIES TIMEOUT 30) 
