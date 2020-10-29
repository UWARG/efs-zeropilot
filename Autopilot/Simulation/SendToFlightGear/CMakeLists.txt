cmake_minimum_required(VERSION 3.5)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_definitions(-Wall -Wextra -g)

find_package(Qt5 COMPONENTS Network REQUIRED)
set(CMAKE_CXX_STANDARD 11)


add_executable(SendToFG main.cpp)
target_link_libraries(SendToFG Qt5::Network)
