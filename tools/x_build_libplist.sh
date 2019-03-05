#!/bin/bash
CURRENT_DIR=$(cd "$(dirname "${BASH_SOURCE-$0}")"; pwd)
VENDOR_DIR=$CURRENT_DIR/../vendor
BUILD_DIR=$VENDOR_DIR/build

mkdir -p $BUILD_DIR
cd $VENDOR_DIR/libplist
make distclean
make clean
./autogen.sh --prefix=$BUILD_DIR --host=arm-none-linux-gnueabihf --without-cython
make
make install

cp -r $BUILD_DIR/include/plist $CURRENT_DIR/../../Vendor/build
cp $BUILD_DIR/lib/libplist.a $CURRENT_DIR/../../Vendor/lib


