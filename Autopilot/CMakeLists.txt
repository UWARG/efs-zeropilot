project(Autopilot C CXX ASM)
cmake_minimum_required(VERSION 3.2.0)

set(ELF_NAME ${PROJECT_NAME}.elf)

add_definitions(
        -DUSE_HAL_LIB
        -DSTM32F7xx
        -DSTM32F765xx
        -DSTM32F765xG
        -DARM_MATH_CM7
)

include_directories(
        Src
        ../Common       
        Drivers/CMSIS/Include
        Drivers/CMSIS/Device/ST/STM32F7xx/Include       
)

# Add ARM math directory for linker to find
link_directories(
        Drivers/CMSIS/Lib/GCC
)

# Libraries
add_subdirectory(Drivers/STM32F7xx_HAL_Driver)
add_subdirectory(Middlewares/Third_Party/FreeRTOS)

# Add files for compilation/project management
file(GLOB_RECURSE C_SOURCES "Src/*.c")
file(GLOB_RECURSE CXX_SOURCES "Src/*.cpp")
file(GLOB_RECURSE USER_INC "Src/*.h")

add_executable(${ELF_NAME} ${C_SOURCES} ${CXX_SOURCES} ${USER_INC} Dev/startup_stm32f765xx.s)

target_link_libraries(${ELF_NAME} STM32_HAL FreeRTOS arm_cortexM7lfdp_math)

# Add project-specific linker flags (.ld script, .map file)
set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/Dev/STM32F765ZGTx_FLASH.ld)
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
