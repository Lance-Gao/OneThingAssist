cmake_minimum_required(VERSION 2.6)

Project(AsrServiceProxy)

set(CMAKE_C_FLAGS "-g -Wall")
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -pthread -ld")

set(SFG_TRACER_PARENT_UTILS_FILE_LISTS)
set(SFG_TRACER_PARENT_UTILS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../log/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/include/opencv4)

aux_source_directory(${SFG_TRACER_PARENT_UTILS_SRC_DIR} SFG_TRACER_PARENT_UTILS_FILE_LISTS)

add_library(utils ${SFG_TRACER_PARENT_UTILS_SRC_DIR}/utils.cpp ${SFG_TRACER_PARENT_UTILS_FILE_LISTS})

find_library(JSON_LIBRARY NAMES jsoncpp json-pb HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(utils crypto ${JSON_LIBRARY})

find_library(OPENCV_LIBRARY_CORE opencv_core HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(utils ${OPENCV_LIBRARY_CORE})

find_library(OPENCV_LIBRARY_HIGH_GUI opencv_highgui HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(utils ${OPENCV_LIBRARY_HIGH_GUI})

find_library(OPENCV_LIBRARY_IMGCODECS opencv_imgcodecs HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(utils ${OPENCV_LIBRARY_IMGCODECS})

find_library(OPENCV_LIBRARY_IMGPROC opencv_imgproc HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(utils ${OPENCV_LIBRARY_IMGPROC})
