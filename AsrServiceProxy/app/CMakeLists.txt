cmake_minimum_required(VERSION 2.6)

Project(AsrServiceProxy)

set(CMAKE_C_FLAGS "-g -Wall")
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")

set(FILE_LISTS)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

aux_source_directory(${SRC_DIR} FILE_LISTS)

add_executable(asr_service_proxy ${FILE_LISTS} ${SRC_DIR}/main.cpp)
