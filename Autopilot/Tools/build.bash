#!/bin/bash
#
# This script builds the ZeroPilot-SW Autopilot subproject
#
# This file is part of WARG's ZeroPilot-SW
#
# Copyright (c) 2018, Waterloo Aerial Robotics Group (WARG)
# All rights reserved.
#
# This software is licensed under a modified version of the BSD 3 clause license
# that should have been included with this software in a file called LICENSE.md
# Otherwise it is available at:
# https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
#

# Enable exit tracing and variable protection
set -o errtrace
set -o nounset

CLEAN=false
RUN_UNIT_TESTS=false
RUN_SIMULATION=false
FLASH=false
BUILD_TYPE="Debug"
GENERATOR="Unix Makefiles"

while getopts "c,s,t,h,f,r" opt; do
    case $opt in
        c)
            CLEAN=true
        ;;
        t)
            RUN_UNIT_TESTS=true
        ;;
        s)
            RUN_SIMULATION=true
        ;;
        f)
            FLASH=true
        ;;
        r)
            BUILD_TYPE="Release"
        ;;
        h|\?)
            printf "%s\n" "Usage: $0 [OPTIONS]"\
                "Script to build the WARG Autopilot project"\
                "    -f                 - flashes the Autopilot after building"\
                "    -c                 - removes previous build files (available for unit test, simulation, and target build) before building"\
                "    -h                 - outputs this message"\
                "    -r                 - Sets the build type to release"\
                "    -t                 - Runs all unit tests"\
                "    -s                 - Runs the simulation and sends its results to flightGear"\
            exit 1
        ;;
    esac
done

# figures out what's available in the used environement
if command -v ninja >/dev/null 2>&1; then
    GENERATOR="Ninja"
elif command -v make >/dev/null 2>&1; then
    GENERATOR="Unix Makefiles"
elif command -v mingw32-make >/dev/null 2>&1; then
    GENERATOR="MinGW Makefiles"
fi


if [[ $RUN_UNIT_TESTS == true ]]; then

    echo "Building Unit tests !"
    echo ""
    echo ""

    BUILD_DIR="testBuild"

    if [[ $CLEAN == true ]]; then
    echo "Cleaning old build environment"
    cmake -E remove_directory $BUILD_DIR
    fi

    cmake -E make_directory $BUILD_DIR
    cmake -E chdir $BUILD_DIR \
      cmake \
        -G "${GENERATOR}" \
        -D KIND_OF_BUILD="UNIT_TESTS"\
        -Wdev\
        -Wdeprecated\
        ../
    cmake --build $BUILD_DIR

    # run all executables in the build directory
    for SCRIPT in $BUILD_DIR/bin/*
    do
        ./$SCRIPT;
    done

elif [[ $RUN_SIMULATION == true ]]; then

    echo "Building Simulation !"
    echo ""
    echo ""

    BUILD_DIR="SimulationBuild"

    if [[ $CLEAN == true ]]; then
        echo "Cleaning old build environment"
        cmake -E remove_directory $BUILD_DIR
        cmake -E make_directory $BUILD_DIR
    fi

    # we only want to run the simulation if things have changed. To check this, we will check if the time stamp of the exe is older than the time stamp at time of build.
    TIME_OF_BUILD=$(date +"%s")

    cmake -E chdir $BUILD_DIR\
      cmake \
        -G "${GENERATOR}" \
        -D KIND_OF_BUILD="SIMULATION"\
        -Wdev\
        -Wdeprecated\
        ../
    cmake --build $BUILD_DIR


    AGE_OF_EXE=$(date -r $BUILD_DIR/sim +"%s")

    if [[ $AGE_OF_EXE -gt $TIME_OF_BUILD ]]; then
        #clear any older simulation results
        cmake -E remove_directory $BUILD_DIR/ActuatorCommands/* 2>/dev/null
        cmake -E remove_directory $BUILD_DIR/SensorOutputs/* 2>/dev/null
        # Note that the program needs to be run from the build directory since it opens and closes files via relative paths.
        cmake -E chdir $BUILD_DIR\
        ./sim
    fi

    echo ""
    echo ""
    echo "Now sending simulation results to flightGear !"
    echo ""
    echo ""

    BUILD_DIR="../../Simulink-Sim/FlightGear/SendToFlightGear/build"

    if [[ $CLEAN == true ]]; then
        cmake -E remove_directory $BUILD_DIR
    fi

    cmake -E make_directory $BUILD_DIR

    cmake -E chdir $BUILD_DIR\
        cmake -G "${GENERATOR}" ..

    cmake --build $BUILD_DIR

    # This program always needs to be run from the autopilot directory
    ./$BUILD_DIR/SendToFG

else
    echo "Building For The Microcontroller !"
    echo ""
    echo ""

    die() {
        echo ""
        echo "Autopilot build FAILED!"
        echo "Error $1 was encountered on line $2."
        exit $1
    }

    # Set up exit condition
    trap 'die $? $LINENO' ERR

    BUILD_DIR="build"

    if [[ $CLEAN == true ]]; then
        echo "Cleaning old build environment"
        cmake -E remove_directory $BUILD_DIR
    fi

    # Prebuild info display
    echo "Building Autopilot..."
    # if [[ $# > 0 ]]; then
    #     echo "with cmake parameters: $@"
    # fi

    # Build commands
    cmake -E make_directory $BUILD_DIR
    cmake -E chdir $BUILD_DIR \
      cmake \
        -G "${GENERATOR}" \
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
        -DCMAKE_TOOLCHAIN_FILE="STM32F765xG.cmake" \
        -D KIND_OF_BUILD="FOR_TARGET"\
        -Wdev\
        -Wdeprecated\
        ../

    cmake --build $BUILD_DIR

    if [[ $FLASH == true ]] ; then
        cmake --build $BUILD_DIR --target install
    fi

    # Final status display
    echo ""
    echo "Autopilot build SUCCESS!"
    exit 0

fi
