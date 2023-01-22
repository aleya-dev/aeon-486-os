#!/bin/bash

rm -rf build
mkdir build
pushd build
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:FILEPATH="cmake/platform/kernel-486.cmake" -DCMAKE_BUILD_TYPE:STRING="release" .. 
popd
