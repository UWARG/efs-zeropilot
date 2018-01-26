#!/bin/bash

mkdir -p build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=STM32F765xG.cmake -G "Unix Makefiles" $1
make && make install