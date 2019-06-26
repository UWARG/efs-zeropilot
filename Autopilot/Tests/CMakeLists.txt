cmake_minimum_required(VERSION 3.2.0)

add_definitions(
  -DUSE_HAL_LIB
  -DSTM32F7xx
  -DSTM32F765xx
  -DSTM32F765xG
  -DARM_MATH_CM7
)

include_directories(
  ../Inc
  ../../Common/Inc
  ../Libraries/CMSIS/Include
  ../Libraries/CMSIS/Device/ST/STM32F7xx/Include
  ../Libraries/STM32F7xx_HAL_Driver/Inc
  ../Lib
  FFF/
)

# gather header and src directories

#for right now, I am only compiling this file. The entire directory will need to be able to compile eventually.
file(GLOB_RECURSE C_SOURCES "../Src/airSpeed.c")

#remove the real main.c file, add the test main instead
 
list(REMOVE_ITEM C_SOURCES "main.c" )
file(GLOB_RECURSE UNIT_TEST_SOURCES "unitTestSrc/*.cpp" )


ADD_DEFINITIONS ( -fpermissive )

# Locate GTest

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Link runTests with what we want to test and the GTest and pthread library

add_executable(runTests ${C_INC} ${C_SOURCES} ${UNIT_TEST_SOURCES} )
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)