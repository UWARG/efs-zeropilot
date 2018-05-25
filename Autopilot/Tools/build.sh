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

THREADS=1
CLEAN=false
TEST=false
FLASH=false
BUILD_TYPE="Debug"

while getopts "c,t,h,f,j:,r" opt; do
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
        j)
            THREADS=$OPTARG
        ;;
        r)
            BUILD_TYPE="Release"
        ;;
        h|\?)
            printf "%s\n" "Usage: $0 [OPTIONS]" \
                "Script to build the WARG Autopilot project"\
                "    -c                 - removes previous build files before building" \
                "    -h                 - outputs this message" \
                "    -t                 - runs tests after building if build is successful"\
                "    -j [threads]       - builds using the given number of threads"\
                "                         default is 1 and is recommended for debugging"\
                "    -r                 - Sets the build type to release"
            exit 1
        ;;
    esac
done


die() {
    echo ""
    echo "Autopilot build FAILED!"
    echo "Error $1 was encountered on line $2."
    exit $1
}

# Set up exit condition
trap 'die $? $LINENO' ERR

# Prebuild info display
echo "Building Autopilot..."
# if [[ $# > 0 ]]; then
#     echo "with cmake parameters: $@"
# fi
echo ""

if [[ $CLEAN == true ]]; then
    echo "Cleaning old build environment"
    rm -rf ./build/*
fi

# Build commands
mkdir -p build
cd build
cmake ../ -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DCMAKE_TOOLCHAIN_FILE="STM32F765xG.cmake" -G "Unix Makefiles"
make -j$THREADS

if [[ $TEST == true ]] ; then
    make test
fi

if [[ $FLASH == true ]] ; then
    make install
fi

# Final status display
echo ""
echo "Autopilot build SUCCESS!"
exit 0
