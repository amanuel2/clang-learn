#!/bin/sh
./cleanup.sh
mkdir build
cd build
cmake -G Ninja ..
ninja classchecker
ninja check-classchecker