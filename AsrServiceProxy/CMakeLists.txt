cmake_minimum_required(VERSION 2.6)

Project(AsrServiceProxy)

# output setting
set (BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/objs/)
set (EXECUTABLE_OUTPUT_PATH ${BINARY_DIR})
set (LIBRARY_OUTPUT_PATH ${BINARY_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
