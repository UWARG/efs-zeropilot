#!/bin/bash
# Requires you have cmake installed
# Sets up the build directory with the appropriate cmake flags for proper compilation
# Needed for IDES like Clion to compile the project

pushd $1

cmake \
    -G "$2" \
    -DCMAKE_BUILD_TYPE="$3" \
    -DCMAKE_TOOLCHAIN_FILE="STM32F765xG.cmake" \
    -Wdev\
    -Wdeprecated\
    ../
popd
