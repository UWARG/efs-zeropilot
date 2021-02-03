# CMakeLists for Mavlink2 library
cmake_minimum_required(VERSION 3.2.0)
project(Mavlink2_lib)

#file(GLOB_RECURSE FREERTOS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/Source/CMSIS_RTOS/*.c")
file(GLOB_RECURSE MAVLINK2_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(${PROJECT_NAME} STATIC ${MAVLINK2_HEADERS}) #add the scattered header files as a library to target


target_include_directories(${PROJECT_NAME} PUBLIC #add the rest of the useful hearder files in folders to target
${CMAKE_CURRENT_SOURCE_DIR}    
${CMAKE_CURRENT_SOURCE_DIR}/common
)
