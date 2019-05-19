cmake_minimum_required(VERSION 3.2.0)
project(Autopilot C CXX ASM)

set(ELF_NAME ${PROJECT_NAME}.elf)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(
  -DUSE_HAL_LIB
  -DSTM32F7xx
  -DSTM32F765xx
  -DSTM32F765xG
  -DARM_MATH_CM7
)

include_directories(
  Inc
  ../Common/Inc
  Libraries/CMSIS/Include
  Libraries/CMSIS/Device/ST/STM32F7xx/Include
  Lib
)

# Add ARM math directory for linker to find
link_directories(
  Libraries/CMSIS/Lib/GCC
)

# Libraries
add_subdirectory(Libraries/STM32F7xx_HAL_Driver)
add_subdirectory(Libraries/FreeRTOS)

file(GLOB_RECURSE C_SOURCES ../Common/Src/*.c ../Common/Src/stm32/*.c "Src/*.c")
file(GLOB_RECURSE CXX_SOURCES ../Common/Src/*.cpp ../Common/Src/stm32/*.cpp "Src/*.cpp")
file(GLOB_RECURSE C_INC "Inc/*.h")
file(GLOB_RECURSE CXX_INC "Inc/*.hpp")

if(NOT NUCLEO)
  set(STARTUP_ASM startup_stm32f765xx.s)
  set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/STM32F765ZGTx_FLASH.ld)
else(NOT NUCLEO)
  set(STARTUP_ASM startup_stm32f767xx.s)
  set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/STM32F767ZITx_FLASH.ld)
endif(NOT NUCLEO)


add_executable(${ELF_NAME} ${C_SOURCES} ${CXX_SOURCES} ${C_INC} ${CXX_INC} ${STARTUP_ASM})

target_link_libraries(${ELF_NAME} STM32_HAL FreeRTOS arm_cortexM7lfdp_math)

# Add project-specific linker flags (.ld script, .map file)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${LINKER_SCRIPT} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map,--cref")

set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

# objcopy the .elf file to .bin file for programming
add_custom_target("${PROJECT_NAME}.bin" ALL
  COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${ELF_NAME}> ${BIN_FILE}
  DEPENDS ${ELF_NAME}
)

# Print size information after compiling
add_custom_command(TARGET ${ELF_NAME}
  POST_BUILD
  COMMENT "Binary size information:"
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${ELF_NAME}>
)

set(INSTALL_CMD "ST-LINK_CLI")
set(INSTALL_OPTS "-c SWD -P \"${BIN_FILE}\" 0x08000000 -NoPrompt -Rst -Run")

install(CODE
  "execute_process(
    COMMAND ${INSTALL_CMD} ${INSTALL_OPTS}
  )"
)

add_custom_target(UPLOAD
        arm-none-eabi-gdb -iex "target remote tcp:127.0.0.1:3333"
        -iex "monitor program $<TARGET_FILE:${PROJECT_NAME}.elf>"
        -iex "monitor reset init"
        -iex "disconnect" -iex "quit")