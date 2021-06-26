cmake_minimum_required(VERSION 2.6)

Project(sfgTracerParent)

set(CMAKE_C_FLAGS "-g -Wall")
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -pthread")

set(SFG_TRACER_PARENT_LOG_FILE_LISTS)
set(SFG_TRACER_PARENT_LOG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utils/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/include)

aux_source_directory(${SFG_TRACER_PARENT_LOG_SRC_DIR} SFG_TRACER_PARENT_LOG_FILE_LISTS)

add_library(log ${SFG_TRACER_PARENT_LOG_SRC_DIR}/log.cpp ${SFG_TRACER_PARENT_LOG_FILE_LISTS})
target_link_libraries(log utils)
