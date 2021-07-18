cmake_minimum_required(VERSION 2.6)

Project(AsrServiceProxy)

set(CMAKE_C_FLAGS "-g -Wall -fpermissive")
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -pthread -fpermissive")

set(FILE_LISTS)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../log/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utils/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/include/opencv4)

aux_source_directory(${SRC_DIR} FILE_LISTS)

add_executable(asr_service_proxy ${FILE_LISTS} ${SRC_DIR}/main.cpp)

target_link_libraries(asr_service_proxy "-Xlinker \"-(\"")

# log and utils
target_link_libraries(asr_service_proxy log utils)

find_library(EXTRA_LIBRARY_COM_LOG comlog HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_LIBRARY_COM_LOG})

find_library(EXTRA_2_LIBRARY_LOG_4_CPP log4cpp HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_2_LIBRARY_LOG_4_CPP})

find_library(BOOST_LIBRARY boost_system boost_filesystem boost_thread HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${BOOST_LIBRARY})
#target_link_libraries(asr_service_proxy boost_system boost_filesystem boost_thread)

find_library(OPENCV_LIBRARY_CORE opencv_core HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${OPENCV_LIBRARY_CORE})

find_library(OPENCV_LIBRARY_HIGH_GUI opencv_highgui HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${OPENCV_LIBRARY_HIGH_GUI})

find_library(OPENCV_LIBRARY_IMGCODECS opencv_imgcodecs HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${OPENCV_LIBRARY_IMGCODECS})

find_library(OPENCV_LIBRARY_IMGPROC opencv_imgproc HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${OPENCV_LIBRARY_IMGPROC})

find_library(EXTRA_3_LIBRARY_DL dl HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_3_LIBRARY_DL})

find_library(EXTRA_2_LIBRARY_LIB_JPEG libopenjp2 HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_2_LIBRARY_LIB_JPEG})

find_library(EXTRA_2_LIBRARY_LIB_TIFF libtiff HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_2_LIBRARY_LIB_TIFF})

find_library(EXTRA_2_LIBRARY_LIB_IPPICV ippicv HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_2_LIBRARY_LIB_IPPICV})

find_library(EXTRA_2_LIBRARY_LIB_ITTNOTIFY ittnotify HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_2_LIBRARY_LIB_ITTNOTIFY})

find_library(EXTRA_2_LIBRARY_LIB_IMF IlmImf HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_2_LIBRARY_LIB_IMF})

find_library(EXTRA_2_LIBRARY_LIB_IPPIW ippiw HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_2_LIBRARY_LIB_IPPIW})

find_library(EXTRA_2_LIBRARY_LIB_ZLIB zlib HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_2_LIBRARY_LIB_ZLIB})

find_library(EXTRA_2_LIBRARY_LIB_LIBJPEGTURBO libjpeg-turbo HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_2_LIBRARY_LIB_LIBJPEGTURBO})

find_library(EXTRA_2_LIBRARY_LIB_LIBPNG libpng HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_2_LIBRARY_LIB_LIBPNG})

find_library(EXTRA_2_LIBRARY_LIB_LIBWEBP libwebp HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib)
target_link_libraries(asr_service_proxy ${EXTRA_2_LIBRARY_LIB_LIBWEBP})

target_link_libraries(asr_service_proxy "-Xlinker \"-)\"")
