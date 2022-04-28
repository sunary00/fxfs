cmake_minimum_required(VERSION 3.8)
project(fxfs C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(${PROJECT_NAME}_SOURCE "${PROJECT_SOURCE_DIR}/src")
file(GLOB_RECURSE ${PROJECT_NAME}_FILES "${${PROJECT_NAME}_SOURCE}/fs*.c")

find_package(FUSE REQUIRED)
include_directories(${FUSE_INCLUDE_DIR})

add_executable(fxfs
        ${${PROJECT_NAME}_FILES} src/main.c)
target_link_libraries(fxfs ${FUSE_LIBRARIES})


# Tests
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    include(CTest)

    if (${BUILD_TESTING})
        add_subdirectory(tests)
    endif()
endif()
