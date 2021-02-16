# Author: Jingting Liu
# CMakeLists for Mavlink library
cmake_minimum_required(VERSION 3.2.0)

PROJECT(Mavlink)



add_subdirectory(Mavlink2_lib)
set_target_properties(Mavlink2_lib PROPERTIES LINKER_LANGUAGE CXX)

add_executable(DECODER Decoder.cpp)
set_target_properties(DECODER PROPERTIES LINKER_LANGUAGE CXX)

#add_executable(ENCODER Encoder.cpp)
#set_target_properties(ENCODER PROPERTIES LINKER_LANGUAGE CXX)
#target_link_libraries(DECODER LINK_PUBLIC Mavlink2_lib)
