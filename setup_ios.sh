#!/bin/bash
SOURCE_ROOT=`pwd`
VENDER_ROOT=$SOURCE_ROOT/..

mkdir -p $VENDER_ROOT/build/include
mkdir -p build
cd build
cmake -GXcode -DBUILD_FOR_IOS=ON ..
xcodebuild
xcodebuild -target install

rm -r $SOURCE_ROOT/../build/include/libSocketKit
mv libSocketKit/include/libSocketKit $SOURCE_ROOT/../build/include/

