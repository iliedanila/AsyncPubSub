#!/bin/sh

cmake -Bbuild/arm/debug -H. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=cross-arm.cmake
cmake --build build/arm/debug