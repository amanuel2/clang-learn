#!/bin/sh
./cleanup.sh
mkdir build
cd build
cmake -G Ninja ..
ninja -j 12