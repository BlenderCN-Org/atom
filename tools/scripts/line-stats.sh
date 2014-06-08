#!/bin/bash

BUILD_DIR="build"

#./waf distclean
./waf configure -t preprocess
./waf
find ${BUILD_DIR} -name '*.o' -exec wc -l {} \;
