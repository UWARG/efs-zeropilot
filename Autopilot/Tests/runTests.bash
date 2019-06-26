#!/bin/bash

cd testBuild/
cmake ../CMakeLists.txt
cd ..
make
./runTests