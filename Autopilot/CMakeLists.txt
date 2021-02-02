cmake_minimum_required(VERSION 3.2.0)
project(Autopilot C CXX ASM)

#checks wether we are building for the microcontroller or to run the PC based unit tests
if (${KIND_OF_BUILD} STREQUAL "FOR_TARGET")

  set(ELF_NAME ${PROJECT_NAME}.elf)

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  add_definitions(
    -DUSE_HAL_LIB
    -DSTM32F7xx
    -DSTM32F765xx
    -DSTM32F765xG
    -DARM_MATH_CM7
  )

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


  get_filename_component(PARENT_DIR ../ ABSOLUTE)

  include_directories(
    Inc
    ${PARENT_DIR}/Common/Inc
    boardfiles/Inc
    boardfiles/Drivers/CMSIS/Include
    boardfiles/Drivers/CMSIS/Device/ST/STM32F7xx/Include
    AttitudeManager
    PathManager/Inc
    TelemetryManager
  )

  # Add ARM math directory for linker to find
  link_directories(
    boardfiles/lib
  )

  # Libraries
  add_subdirectory(boardfiles/Drivers/STM32F7xx_HAL_Driver)
  add_subdirectory(boardfiles/Middlewares/Third_Party/FreeRTOS)

  file(GLOB_RECURSE C_SOURCES ../Common/Src/*.c ../Common/Src/stm32/*.c "Src/*.c" AttitudeManager/*.c boardfiles/Src/*.c)
  file(GLOB_RECURSE CXX_SOURCES ../Common/Src/*.cpp ../Common/Src/stm32/*.cpp "Src/*.cpp" AttitudeManager/*.cpp PathManager/Src/*.cpp TelemetryManager/*.cpp)

  if(NOT NUCLEO)
    set(STARTUP_ASM startup_stm32f765xx.s)
    set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/STM32F765ZGTx_FLASH.ld)
  else(NOT NUCLEO)
    set(STARTUP_ASM startup_stm32f767xx.s)
    set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/STM32F767ZITx_FLASH.ld)
  endif(NOT NUCLEO)


  add_executable(${ELF_NAME} ${C_SOURCES} ${CXX_SOURCES} ${STARTUP_ASM})

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

elseif(${KIND_OF_BUILD} STREQUAL "UNIT_TESTS")

  add_definitions(-DUNIT_TESTING)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fms-extensions -Wall -Wextra -g")

  # Google test was built for c++11
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

  # Required to allow us to point to the gtest directory using an environment variable. (This has only been an issue on the windows CI)
  if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
  endif()

  include_directories(
    SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/Test/vendor/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc/
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/TelemetryManager
  )

  # Let cmake know where to find all the find<Package>.cmake files
  set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Test/vendor/FindCmakeModules")

  # Locate GTest
  find_package(GTest REQUIRED)
  include_directories(${GTEST_INCLUDE_DIRS})

  # Locate GMock
  find_package(GMock REQUIRED)
  include_directories(${GMOCK_INCLUDE_DIRS})

  #instruct cmake to store all binaries in their own directory
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

  # Collect the main function that runs our unit tests. It will be common for all executables
  set(UNIT_TEST_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/Test/testMain.cpp)


######### Attitude manager fsm

  set(ATTITUDE_MANAGER_FSM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/attitudeManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/attitudeStateClasses.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/PID.cpp
  )

  set(ATTITUDE_MANAGER_FSM_UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/AttitudeManager/Test_Fsm.cpp
  )

  add_executable(attitudeManagerFSM ${ATTITUDE_MANAGER_FSM_SOURCES} ${ATTITUDE_MANAGER_FSM_UNIT_TEST_SOURCES} ${UNIT_TEST_MAIN})
  target_link_libraries(attitudeManagerFSM ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} pthread)

#########

######### Attitude manager modules

  set(ATTITUDE_MANAGER_MODULES_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/OutputMixing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/SensorFusion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/MadgwickAHRS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/fetchSensorMeasurementsMode.cpp
  )

  set(ATTITUDE_MANAGER_MODULES_UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/AttitudeManager/Test_SensorFusion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/AttitudeManager/Test_OutputMixing.cpp
  )

  add_executable(attitudeManagerModules ${ATTITUDE_MANAGER_MODULES_SOURCES} ${ATTITUDE_MANAGER_MODULES_UNIT_TEST_SOURCES} ${UNIT_TEST_MAIN})
  target_link_libraries(attitudeManagerModules ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} pthread)

#########

######### Path manager modules

  set(PATH_MANAGER_MODULES_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/waypointManager.cpp
  )

  set(PATH_MANAGER_MODULES_UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/Test_WaypointManager.cpp
  )

  add_executable(pathManagerModules ${PATH_MANAGER_MODULES_SOURCES} ${PATH_MANAGER_MODULES_UNIT_TEST_SOURCES} ${UNIT_TEST_MAIN})
  target_link_libraries(pathManagerModules ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} pthread)

#########

######### Telemetry manager fsm

  set(TELEMETRY_MANAGER_FSM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/TelemetryManager/telemetryManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TelemetryManager/telemetryStateClasses.cpp
  )

  set(TELEMETRY_MANAGER_FSM_UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
  )

  add_executable(telemetryManagerFSM ${TELEMETRY_MANAGER_FSM_SOURCES} ${TELEMETRY_MANAGER_FSM_UNIT_TEST_SOURCES} ${UNIT_TEST_MAIN})
  target_link_libraries(telemetryManagerFSM ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} pthread)

#########

######### Free standing modules that are not application specific

  set(FREE_STANDING_MODULES_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/PID.cpp
  )

  set(FREE_STANDING_MODULES_UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/Test_PID.cpp
  )

  add_executable(freeStandingModules ${FREE_STANDING_MODULES_SOURCES} ${FREE_STANDING_MODULES_UNIT_TEST_SOURCES} ${UNIT_TEST_MAIN})
  target_link_libraries(freeStandingModules ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} pthread)

#########


elseif(${KIND_OF_BUILD} STREQUAL "SIMULATION")

  add_definitions(-DSIMULATION)

  set(SIM_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../Simulink-Sim/Infinion)

  # allow debugging if required
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)


  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc/
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager

    ${SIM_ROOT_DIR}/GeneratedCCode/SimulinkModel/Infinion_grt_rtw
    ${SIM_ROOT_DIR}/GeneratedCCode/R2020a/extern/include
    ${SIM_ROOT_DIR}/GeneratedCCode/R2020a/rtw/c/src
    ${SIM_ROOT_DIR}/GeneratedCCode/R2020a/rtw/c/src/ext_mode/common
    ${SIM_ROOT_DIR}/GeneratedCCode/R2020a/simulink/include
  )

  add_custom_target(make-ActuatorCommand-dir ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/ActuatorCommands")
  add_custom_target(make-SensorOutput-dir ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/SensorOutputs")


  set(ATTITUDE_MANAGER_SIM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Simulation/SimDriver/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/attitudeManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/attitudeStateClasses.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/OutputMixing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/SensorFusion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/MadgwickAHRS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/PID.cpp
  )

  set(AUTOPILOT_INTERCEPT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Simulation/Intercepts/GetFromPathManager_Intercept.cpp
    )

  set(ZERO_PILOT_HOOK_SOURCES
  ${SIM_ROOT_DIR}/ZeroPilotSwHooks/SendToSafetyIntercept.cpp
  ${SIM_ROOT_DIR}/ZeroPilotSwHooks/AirspeedSensorIntercept.cpp
  ${SIM_ROOT_DIR}/ZeroPilotSwHooks/IMUSensorIntercept.cpp
  )

  set(SIMULINK_GENERATED_SOURCES
  ${SIM_ROOT_DIR}/GeneratedCCode/SimulinkModel/Infinion_grt_rtw/Infinion.cpp
  ${SIM_ROOT_DIR}/GeneratedCCode/SimulinkModel/Infinion_grt_rtw/Infinion_data.cpp
  ${SIM_ROOT_DIR}/GeneratedCCode/SimulinkModel/Infinion_grt_rtw/rt_nonfinite.cpp
  ${SIM_ROOT_DIR}/GeneratedCCode/SimulinkModel/Infinion_grt_rtw/rtGetNaN.cpp
  ${SIM_ROOT_DIR}/GeneratedCCode/SimulinkModel/Infinion_grt_rtw/rtGetInf.cpp
  )

  add_executable(sim ${ATTITUDE_MANAGER_SIM_SOURCES} ${AUTOPILOT_INTERCEPT_SOURCES} ${ZERO_PILOT_HOOK_SOURCES} ${SIMULINK_GENERATED_SOURCES})

  # Link with the math library
  target_link_libraries(sim m)

endif()
