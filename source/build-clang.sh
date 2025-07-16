#!/bin/bash
export CC=clang
export CXX=clang++
export VCPKG_ROOT=/home/flupppi/dev/vcpkg

SRC_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cmake -S "$SRC_DIR" -B "$SRC_DIR/build-clang" -G Ninja \
  -DCMAKE_CXX_FLAGS="--stdlib=libc++" \
  -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++" \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_FEATURE_FLAGS=manifests

cmake --build "$SRC_DIR/build-clang"
