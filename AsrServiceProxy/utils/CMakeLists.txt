cmake_minimum_required(VERSION 2.6)

Project(sfgTracerParent)

set(CMAKE_C_FLAGS "-g -Wall")
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -pthread -ld")

set(SFG_TRACER_PARENT_UTILS_FILE_LISTS)
set(SFG_TRACER_PARENT_UTILS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../log/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/include)

aux_source_directory(${SFG_TRACER_PARENT_UTILS_SRC_DIR} SFG_TRACER_PARENT_UTILS_FILE_LISTS)

add_library(utils ${SFG_TRACER_PARENT_UTILS_SRC_DIR}/utils.cpp ${SFG_TRACER_PARENT_UTILS_FILE_LISTS})

find_library(JSON_LIBRARY NAMES jsoncpp json-pb HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(utils crypto ${JSON_LIBRARY})
