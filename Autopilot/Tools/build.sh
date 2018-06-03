#!/bin/bash

# This script builds the ZeroPilot-SW Autopilot subproject

die() {
    echo ""
    echo "Autopilot build FAILED!"
    echo "Error $1 was encountered."
    exit $1
}

# Prebuild info display
echo "Building Autopilot..."
if [[ $# > 0 ]]; then
    echo "with cmake parameters: $@"
fi
echo ""

# Build commands
mkdir -p build || die $?
cd build || die $?
cmake ../ -DCMAKE_TOOLCHAIN_FILE=STM32F765xG.cmake -G "Unix Makefiles" $@ || die $?
make || die $?

# Final status display
echo ""
echo "Autopilot build SUCCESS!"
exit 0
