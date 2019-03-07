#!/bin/bash
CURRENT_DIR=$(cd "$(dirname "${BASH_SOURCE-$0}")"; pwd)
SOURCE_ROOT=$CURRENT_DIR/..
VENDER_ROOT=$SOURCE_ROOT/..

cd $SOURCE_ROOT
mkdir -p $VENDER_ROOT/build/include
rm -r build
mkdir -p build
cd build
cmake -GXcode -DBUILD_FOR_IOS=ON ..

