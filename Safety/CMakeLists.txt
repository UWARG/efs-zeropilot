cmake_minimum_required(VERSION 3.2.0)
project(Safety C CXX ASM)

set(ELF_NAME ${PROJECT_NAME}.elf)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(
  -DUSE_HAL_LIB
  -DSTM32F030xC
)

include_directories(
    boardfiles/Inc
    boardfiles/Drivers/STM32F0xx_HAL_Driver/Inc
    boardfiles/Drivers/CMSIS/Device/ST/STM32F0xx/Include
    boardfiles/Drivers/CMSIS/Include
    Inc
)

# Libraries
add_subdirectory(Libraries/STM32F0xx_HAL_Driver)

# Tests
#add_subdirectory(Tests)
file(GLOB_RECURSE C_SOURCES  "boardfiles/Drivers/STM32F0xx_HAL_Driver/Src*.c" "boardfiles/Src/*.c")
file(GLOB_RECURSE CXX_SOURCES "boardfiles/Src/*.cpp" "Src/*.cpp")


if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/boardfiles/Src/main.c")
  message( FATAL_ERROR "Hey Bub, you screwed up, but it's all good. You see, CubeMx generates a main.c rather than a main.cpp, so before generation, you need to change the extension of boardfiles/Src/main.cpp to boardfiles/Src/main.c, allowing cubeMX to update your main file. Then, after generation, rename the file to boardfiles/Src/main.cpp." )
endif()

set(STARTUP_ASM startup_stm32f030xc.s)
set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/STM32F030RCTx_FLASH.ld)

add_executable(${ELF_NAME} ${C_SOURCES} ${CXX_SOURCES} ${STARTUP_ASM})

target_link_libraries(${ELF_NAME} STM32_HAL)

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

# the following is windows only
set(INSTALL_CMD "ST-LINK_CLI")
set(INSTALL_OPTS "-c SWD -P \"${BIN_FILE}\" 0x08000000 -NoPrompt -Rst -Run")

install(CODE
  "execute_process(
    COMMAND ${INSTALL_CMD} ${INSTALL_OPTS}
  )"
)

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Debug build.")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    message("Release build.")
ELSE()
    message("Some other build type.")
ENDIF()

add_custom_target(UPLOAD
        arm-none-eabi-gdb -iex "target remote tcp:127.0.0.1:3333"
        -iex "monitor program $<TARGET_FILE:${PROJECT_NAME}.elf>"
        -iex "monitor reset init"
        -iex "disconnect" -iex "quit")
