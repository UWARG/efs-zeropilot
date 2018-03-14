#!/bin/bash

# Enable exit tracing and variable protectionset --o 
set -o errtrace
set -o nounset

# This script builds the ZeroPilot-SW Autopilot subproject

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
if [[ $# > 0 ]]; then
    echo "with cmake parameters: $@"
fi
echo ""

# Build commands
mkdir -p build
cd build
cmake ../ -DCMAKE_TOOLCHAIN_FILE="STM32F765xG.cmake" -G "Unix Makefiles" $@
make

# Final status display
echo ""
echo "Autopilot build SUCCESS!"
exit 0
