cmake_minimum_required(VERSION 3.2.0)
project(Tests CXX)

find_package(UnitTest++ REQUIRED)

# enable verbosity in linker. Useful for debugging
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -v")

file(GLOB_RECURSE CXX_SOURCES "Src/*.cpp")

include_directories(${UTPP_INCLUDE_DIRS})

add_executable(safety_tests ${CXX_SOURCES})

target_link_libraries(safety_tests UnitTest++)