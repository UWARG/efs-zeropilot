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

CLEAN=true

BUILD_DIR="testBuild"

if [[ $CLEAN == true ]]; then
    echo "Cleaning old build environment"
    cmake -E remove_directory $BUILD_DIR
fi


# Build commands
cmake -E make_directory $BUILD_DIR
cmake -E chdir $BUILD_DIR \
  cmake \
    -Wdev\
    -Wdeprecated\
    ../

cmake --build $BUILD_DIR
