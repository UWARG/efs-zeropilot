cmake_minimum_required(VERSION 3.2.0)
project(Autopilot C CXX ASM)

#checks wether we are building for the microcontroller or to run the PC based unit tests
if (${KIND_OF_BUILD} STREQUAL "FOR_TARGET")

  add_definitions(-DTARGET_BUILD)

  set(ELF_NAME ${PROJECT_NAME}.elf)

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  add_definitions(
    -DUSE_HAL_LIB
    -DSTM32F7xx
    -DSTM32F765xx
    -DSTM32F765xG
    -DARM_MATH_CM7
  )

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")


  get_filename_component(PARENT_DIR ../ ABSOLUTE)

  include_directories(
    Inc
    ${PARENT_DIR}/Common/Inc
    boardfiles/Inc
    boardfiles/Drivers/CMSIS/Include
    boardfiles/Drivers/CMSIS/Device/ST/STM32F7xx/Include
    AttitudeManager/Inc
    PathManager/Inc
    TelemetryManager/Inc
    CControl/Inc
    SensorFusion/Inc
  )

  # Add ARM math directory for linker to find
  link_directories(
    boardfiles/lib
  )

  # Libraries
  add_subdirectory(boardfiles/Drivers/STM32F7xx_HAL_Driver)
  add_subdirectory(boardfiles/Middlewares/Third_Party/FreeRTOS)
  add_subdirectory(DataStructures)

  file(GLOB_RECURSE C_SOURCES ../Common/Src/*.c ../Common/Src/stm32/*.c "Src/*.c" AttitudeManager/Src/*.c boardfiles/Src/*.c CControl/Src/*.c)
file(GLOB_RECURSE CXX_SOURCES ../Common/Src/*.cpp ../Common/Src/stm32/*.cpp "Src/*.cpp" AttitudeManager/Src/*.cpp PathManager/Src/*.cpp TelemetryManager/Src/*.cpp SensorFusion/Src/*.cpp SensorFusion/Src/*.c)
  if(NOT NUCLEO)
    set(STARTUP_ASM startup_stm32f765xx.s)
    set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/STM32F765ZGTx_FLASH.ld)
  else(NOT NUCLEO)
    set(STARTUP_ASM startup_stm32f767xx.s)
    set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/STM32F767ZITx_FLASH.ld)
  endif(NOT NUCLEO)


  add_executable(${ELF_NAME} ${C_SOURCES} ${CXX_SOURCES} ${STARTUP_ASM})

  target_link_libraries(${ELF_NAME} STM32_HAL FreeRTOS DataStructures arm_cortexM7lfdp_math)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/TelemetryManager/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/DataStructures/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/CControl/Inc/
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorFusion/Inc
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
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/Src/attitudeManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/Src/attitudeStateClasses.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/PID.cpp
  )

  set(ATTITUDE_MANAGER_FSM_UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/AttitudeManager/Test_AttitudeManager_Fsm.cpp
  )

  add_executable(attitudeManagerFSM ${ATTITUDE_MANAGER_FSM_SOURCES} ${ATTITUDE_MANAGER_FSM_UNIT_TEST_SOURCES} ${UNIT_TEST_MAIN})
  target_link_libraries(attitudeManagerFSM ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} pthread)

#########

######### Attitude manager modules

  set(ATTITUDE_MANAGER_MODULES_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/Src/OutputMixing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorFusion/Src/SensorFusion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/Src/MahonyAHRS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CControl/Src/mul.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CControl/Src/tran.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CControl/Src/inv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CControl/Src/lup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/SensorFusion/Test_IMU.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/SensorFusion/Test_GPS.cpp
  )

  set(ATTITUDE_MANAGER_MODULES_UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/AttitudeManager/Test_OutputMixing.cpp
  )

  add_executable(attitudeManagerModules ${ATTITUDE_MANAGER_MODULES_SOURCES} ${ATTITUDE_MANAGER_MODULES_UNIT_TEST_SOURCES} ${UNIT_TEST_MAIN})
  target_link_libraries(attitudeManagerModules ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} pthread)

#########

######### Path manager fsm

  set(PATH_MANAGER_FSM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/pathManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/pathStateClasses.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/waypointManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/cruisingState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/landingTakeoffManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DataStructures/Src/vectorClass.cpp
  )

  set(PATH_MANAGER_FSM_UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/PathManager/Test_PathManager_Fsm.cpp
  )

  add_executable(pathManagerFsm ${PATH_MANAGER_FSM_SOURCES} ${PATH_MANAGER_FSM_UNIT_TEST_SOURCES} ${UNIT_TEST_MAIN})
  target_link_libraries(pathManagerFsm ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} pthread)

#########

######### Path manager modules

  set(PATH_MANAGER_MODULES_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/waypointManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/cruisingState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/landingTakeoffManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DataStructures/Src/vectorClass.cpp
  )

  set(PATH_MANAGER_MODULES_UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/PathManager/Test_WaypointManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/PathManager/Test_CruisingState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/LandingTakeoffManager/Test_Landing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/LandingTakeoffManager/Test_Takeoff.cpp
  )

  add_executable(pathManagerModules ${PATH_MANAGER_MODULES_SOURCES} ${PATH_MANAGER_MODULES_UNIT_TEST_SOURCES} ${UNIT_TEST_MAIN})
  target_link_libraries(pathManagerModules ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} pthread)

#########

######### Telemetry manager fsm

  set(TELEMETRY_MANAGER_FSM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/TelemetryManager/Src/telemetryManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TelemetryManager/Src/telemetryStateClasses.cpp
  )

  set(TELEMETRY_MANAGER_FSM_UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/TelemetryManager/Test_TelemetryManager_Fsm.cpp
  )

  add_executable(telemetryManagerFSM ${TELEMETRY_MANAGER_FSM_SOURCES} ${TELEMETRY_MANAGER_FSM_UNIT_TEST_SOURCES} ${UNIT_TEST_MAIN})
  target_link_libraries(telemetryManagerFSM ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} pthread)

#########

#########  SensorFusion manager

set(SENSOR_FUSION_MODULES_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorFusion/Src/SensorFusion.cpp
  )  

  set(SENSOR_FUSION_MODULES_UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/SensorFusion/Test_SF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/Src/MahonyAHRS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CControl/Src/mul.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CControl/Src/tran.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CControl/Src/inv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CControl/Src/lup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/SensorFusion/Test_IMU.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/SensorFusion/Test_GPS.cpp
  )

  add_executable(sensorFusionModules ${SENSOR_FUSION_MODULES_SOURCES} ${SENSOR_FUSION_MODULES_UNIT_TEST_SOURCES} ${UNIT_TEST_MAIN})
  target_link_libraries(sensorFusionModules ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} pthread)

######### Vector Class

  set(VECTOR_CLASS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/DataStructures/Src/vectorClass.cpp
  )

  set(VECTOR_CLASS_UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Src/DataStructures/Test_Vector_Class.cpp
  )

  add_executable(vectorClassTest ${VECTOR_CLASS_SOURCES} ${VECTOR_CLASS_UNIT_TEST_SOURCES} ${UNIT_TEST_MAIN})
  target_link_libraries(vectorClassTest ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} pthread)

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

if( NOT EXISTS "${SIM_ROOT_DIR}/GeneratedCCode/Headers")
  message( FATAL_ERROR "Evenin buddy. So looks like you've got an issue here. 1 of 2 things happened. First potential issue is you've forgotten to copy the unzipped GeneratedCCode directory into the Infinion folder, go and do that if you haven't. If that's done then here's your problem: see, when Simulink generates code for you it will name one of the directories inside the GeneratedCCode directory after the current matlab version. Cmake can't keep up with that as things change over the years so you're going to have to rename the R2021a (or whatever version your'e at) to be Headers. Anyway, I'll leave you alone now. Good luck with your cool simulation, I can only imagine what sort of crazy shit you're trying to get working :D." )
endif()

  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc/
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/TelemetryManager/Inc

    ${SIM_ROOT_DIR}/GeneratedCCode/SimulinkModel/Infinion_grt_rtw
    ${SIM_ROOT_DIR}/GeneratedCCode/Headers/extern/include
    ${SIM_ROOT_DIR}/GeneratedCCode/Headers/rtw/c/src
    ${SIM_ROOT_DIR}/GeneratedCCode/Headers/rtw/c/src/ext_mode/common
    ${SIM_ROOT_DIR}/GeneratedCCode/Headers/simulink/include
  )

  add_custom_target(make-ActuatorCommand-dir ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/ActuatorCommands")
  add_custom_target(make-SensorOutput-dir ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/SensorOutputs")
  add_custom_target(make-DebugLogs-dir ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/DebugLogs")

  set(SIM_MAIN
    ${CMAKE_CURRENT_SOURCE_DIR}/Simulation/SimDriver/SimMain.cpp
  )

  set(PATH_MANAGER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/pathManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/pathStateClasses.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/waypointManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/landingTakeoffManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/cruisingState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PathManager/Src/AutoSteer.cpp
  )

  set(ATTITUDE_MANAGER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/Src/attitudeManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/Src/attitudeStateClasses.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/Src/OutputMixing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AttitudeManager/Src/MahonyAHRS.cpp
  )

  set(OTHER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/PID.cpp
  )

  set(AUTOPILOT_INTERCEPT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Simulation/Intercepts/CommsBetweenStateMachines_Intercept.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Simulation/Intercepts/CommsWithTelemetry_intercept_user.cpp
  )

  set(ZERO_PILOT_HOOK_SOURCES
    ${SIM_ROOT_DIR}/ZeroPilotSwHooks/SensorFusionIntercept.cpp
    ${SIM_ROOT_DIR}/ZeroPilotSwHooks/SendToSafetyIntercept.cpp
  )

  set(SIMULINK_GENERATED_SOURCES
  ${SIM_ROOT_DIR}/GeneratedCCode/SimulinkModel/Infinion_grt_rtw/Infinion.cpp
  ${SIM_ROOT_DIR}/GeneratedCCode/SimulinkModel/Infinion_grt_rtw/Infinion_data.cpp
  ${SIM_ROOT_DIR}/GeneratedCCode/SimulinkModel/Infinion_grt_rtw/rt_nonfinite.cpp
  ${SIM_ROOT_DIR}/GeneratedCCode/SimulinkModel/Infinion_grt_rtw/rtGetNaN.cpp
  ${SIM_ROOT_DIR}/GeneratedCCode/SimulinkModel/Infinion_grt_rtw/rtGetInf.cpp
  )

  # Libraries
  add_subdirectory(DataStructures)


  add_executable(sim ${SIM_MAIN} ${PATH_MANAGER_SOURCES} ${ATTITUDE_MANAGER_SOURCES} ${AUTOPILOT_INTERCEPT_SOURCES} ${ZERO_PILOT_HOOK_SOURCES} ${OTHER_SOURCES} ${SIMULINK_GENERATED_SOURCES})

  # Link with the math library
  target_link_libraries(sim m DataStructures)

endif()
