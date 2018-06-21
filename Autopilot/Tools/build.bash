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
TEST=false
FLASH=false
BUILD_TYPE="Debug"
GENERATOR="Unix Makefiles"

while getopts "c,t,h,f,r" opt; do
    case $opt in
        c)
            CLEAN=true
        ;;
        t)
            TEST=true
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
                "    -c                 - removes previous build files before building"\
                "    -h                 - outputs this message"\
                "    -t                 - runs tests after building if build is successful"\
                "    -r                 - Sets the build type to release"
            exit 1
        ;;
    esac
done

if command -v ninja >/dev/null 2>&1; then
    GENERATOR="Ninja"
elif command -v make >/dev/null 2>&1; then
    GENERATOR="Unix Makefiles"
elif command -v mingw32-make >/dev/null 2>&1; then
    GENERATOR="MinGW Makefiles"
fi

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
    ../

cmake --build $BUILD_DIR

if [[ $TEST == true ]] ; then
    cmake --build $BUILD_DIR --target test
fi

if [[ $FLASH == true ]] ; then
    cmake --build $BUILD_DIR --target install
fi

# Final status display
echo ""
echo "Autopilot build SUCCESS!"
exit 0
