cmake_minimum_required(VERSION 3.2.0)
project(Tests)

find_package(UnitTest++ REQUIRED)

set(CMAKE_SYSTEM_NAME ${CMAKE_HOST_SYSTEM_NAME})
set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# enable verbosity in linker
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -v")

#
#set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu11" CACHE INTERNAL "")
#set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++17 -fno-rtti -fno-exceptions -fno-unwind-tables" CACHE INTERNAL "")
#set(CMAKE_ASM_FLAGS "${COMMON_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "")
#set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -specs=nosys.specs -specs=nano.specs -Wl,-gc-sections" CACHE INTERNAL "")


file(GLOB_RECURSE C_SOURCES "Src/*.c")
file(GLOB_RECURSE CXX_SOURCES "Src/*.cpp")
file(GLOB_RECURSE C_INC "Inc/*.h")
file(GLOB_RECURSE CXX_INC "Inc/*.hpp")

include_directories(${UTPP_INCLUDE_DIRS})


add_executable(safety_tests ${C_SOURCES} ${CXX_SOURCES} ${C_INC} ${CXX_INC})

target_link_libraries(safety_tests UnitTest++)

# specify compiler
#set(CMAKE_C_COMPILER gcc)
#set(CMAKE_CXX_COMPILER g++)
#set(CMAKE_ASM_COMPILER gcc)
#set(CMAKE_OBJCOPY arm-none-eabi-objcopy CACHE INTERNAL "")
#set(CMAKE_SIZE arm-none-eabi-size CACHE INTERNAL "")


# ARM compiler options
#set(MCU_CPU cortex-m0)
#set(MCU_FLOAT_ABI soft) # since we don't have a floating point unit on this chip. Otherwise should be hard
#set(MCU_FLAGS "-mcpu=${MCU_CPU} -mthumb -mfloat-abi=${MCU_FLOAT_ABI}")
#
#set(COMMON_FLAGS "${MCU_FLAGS} -Wall -Wextra -Wno-unused-parameter -ffunction-sections -fdata-sections")
#

#
## insert DEBUG flag if we're compiling under debug
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -DDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -DDEBUG")
#
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
