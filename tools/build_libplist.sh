#!/bin/bash
CURRENT_DIR=`pwd`
VENDOR_DIR=$CURRENT_DIR/../vendor
BUILD_DIR=$VENDOR_DIR/build

mkdir -p $BUILD_DIR
cd $VENDOR_DIR/libplist
./autogen.sh --prefix=$BUILD_DIR --host=arm-none-linux-gnueabihf --without-cython
make
make install

