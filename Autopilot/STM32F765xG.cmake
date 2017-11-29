set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
set(CMAKE_C_COMPILER    arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER  arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_OBJCOPY       arm-none-eabi-objcopy)
set(CMAKE_LINKER        arm-none-eabi-gcc)

# ARM compiler options
set(MCU_CPU cortex-m7)
set(MCU_FPU fpv5-d16)
set(MCU_FLOAT_ABI hard)


set(MCU_FLAGS "-mcpu=${MCU_CPU} -mthumb -mfloat-abi=${MCU_FLOAT_ABI}")
if (MCU_FLOAT_ABI STREQUAL hard)
    set(MCU_FLAGS "${MCU_FLAGS} -mfpu=${MCU_FPU}")
endif ()

set(COMMON_FLAGS "${MCU_FLAGS} -Wall -ffunction-sections -fdata-sections")

set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu11" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=gnu++11" CACHE INTERNAL "")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "")
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -specs=nosys.specs -specs=nano.specs -Wl,-gc-sections" CACHE INTERNAL "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
