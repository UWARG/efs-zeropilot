#!/bin/bash
#
# This script builds and runs the unit tests of the ZeroPilot-SW Autopilot subproject
#
# This file is part of WARG's ZeroPilot-SW
#
# Copyright (c) 2019, Waterloo Aerial Robotics Group (WARG)
# All rights reserved.
#
# This software is licensed under a modified version of the BSD 3 clause license
# that should have been included with this software in a file called LICENSE.md
# Otherwise it is available at:
# https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
#

CLEAN=false


BUILD_DIR="testBuild"


if [[ $CLEAN == true ]]; then
    echo "Cleaning old build environment"
    cmake -E remove_directory $BUILD_DIR
fi

cmake -E make_directory $BUILD_DIR
cmake -E chdir $BUILD_DIR

cmake ../CMakeLists.txt

cmake --build $BUILD_DIR



