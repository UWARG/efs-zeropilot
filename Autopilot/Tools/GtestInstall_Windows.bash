#!/bin/bash

choco install -y mingw
choco install -y wget
choco install -y 7zip.install

mkdir GtestTemp
cd GtestTemp

wget https://github.com/google/googletest/archive/release-1.10.0.zip
7z x release-1.10.0.zip -r
cd googletest-release-1.10.0
mkdir build
cd build
cmake -G"Unix Makefiles" ..
make
make install

setx GTEST_ROOT "C:\\Program Files (x86)\\googletest-distribution\\"
setx GMOCK_ROOT "C:\\Program Files (x86)\\googletest-distribution\\"

cd ../../..
rm -r GtestTemp
