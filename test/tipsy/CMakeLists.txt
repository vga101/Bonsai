cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

add_executable(create_tipsy create_tipsy.cpp)
add_executable(ascii_to_tipsy ascii_to_tipsy.cpp)
add_executable(read_tipsy read_tipsy.cpp)
add_executable(split_tipsy split_tipsy.cpp)
add_executable(reduce_tipsy reduce_tipsy.cpp)

# provide git version to source
# http://xit0.org/2013/04/cmake-use-git-branch-and-commit-details-in-project/
execute_process(
    COMMAND git log -1 --format=%H
    #COMMAND git describe --always --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions("-DGIT_VERSION=\"${GIT_VERSION}\"")
# alternative via header:
#configure_file(
#    # template file
#    ${CMAKE_SOURCE_DIR}/version.h.in
#    # output file
#    ${CMAKE_BINARY_DIR}/generated/version.h
#)
#include_directories(${CMAKE_BINARY_DIR}/generated)
