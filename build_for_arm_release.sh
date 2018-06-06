#!/bin/sh

cmake -Bbuild/arm/release -H. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cross-arm.cmake
cmake --build build/arm/release