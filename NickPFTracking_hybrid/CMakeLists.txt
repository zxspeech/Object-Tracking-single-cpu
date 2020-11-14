cmake_minimum_required(VERSION 3.7)#根据你的cmake版本定
project(NickPFTracking_RGBMPI_mini)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp particleFilter2D.h mySet.h)
set(opencv)
set(opencv2)
set(INC_DIR /usr/local/include/opencv2)
set(INC_DIRR /usr/local/include/opencv)

set(CMAKE_C_COMPILER /usr/local/bin/mpicc)
set(CMAKE_CXX_COMPILER /usr/local/bin/mpic++)
set(CMAKE_CXX_COMPILER /usr/local/bin/mpicxx)

include_directories(${INC_DIR})
include_directories(${INC_DIRR})
link_directories(${LINK_DIR})
link_directories(${LINK_DIRR})

add_executable(NickPFTracking_RGBMPI_mini ${SOURCE_FILES} particleFilter2D.h mySet.h)
find_package(OpenCV REQUIRED)
target_link_libraries(NickPFTracking_RGBMPI_mini ${OpenCV_LIBS})