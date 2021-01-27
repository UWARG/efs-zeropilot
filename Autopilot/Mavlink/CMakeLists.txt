# Author: Jingting Liu
# CMakeLists for Mavlink library
cmake_minimum_required(VERSION 3.2.0)
project(Mavlink2)

file(GLOB_RECURSE MAVLINK_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Mavlink2_lib/*.h")

add_library(${PROJECT_NAME} STATIC ${MAVLINK_HEADERS})


add_executable(DECODER Decoder.cpp)

target_link_libraries(DECODER MAVLINK_HEADERS)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Mavlink2_lib
)