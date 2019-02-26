#!/bin/bash
CURRENT_DIR=`pwd`
VENDOR_DIR=$CURRENT_DIR/../vendor
BUILD_DIR=$VENDOR_DIR/build

cd $VENDOR_DIR/libplist
./autogen.sh --prefix=$BUILD_DIR --without-cython
make
make install

