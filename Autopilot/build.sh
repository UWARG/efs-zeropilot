#!/bin/bash

# This script builds the ZeroPilot-SW Autopilot subproject

echo "Building Autopilot..."

mkdir -p build
cd build
cmake ../ -DCMAKE_TOOLCHAIN_FILE=STM32F765xG.cmake -G "Unix Makefiles"
make

if [[ $? != 0 ]]; then
    echo ""
    echo "Autopilot build FAILED!"
    echo "Error $? encountered by make"
    exit $?
fi

echo ""
echo "Autopilot build SUCCESS!"
exit 0
