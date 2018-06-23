# CMakeLists for STM32 HAL library
cmake_minimum_required(VERSION 3.2.0)
project(STM32_HAL C)

file(GLOB_RECURSE HAL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.c")
file(GLOB_RECURSE HAL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Inc/*.h")

add_library(${PROJECT_NAME} STATIC ${HAL_SOURCES} ${HAL_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)
