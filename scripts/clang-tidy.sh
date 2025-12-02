# runs clang-tidy
# clang-tidy.sh <compiler> <build_mode>
# eg. clang-tidy.sh gcc release

#!bin/bash

GCC_DEBUG_BUILD_DIR="build/x86_64-linux-gcc/debug"
GCC_RELEASE_BUILD_DIR="build/x86_64-linux-gcc/release"
CLANG_DEBUG_BUILD_DIR="build/x86_64-linux-clang/debug"
CLANG_RELEASE_BUILD_DIR="build/x86_64-linux-clang/release"

CLANG_TIDY_CONFIG=".clang-tidy"

FILES=$(find src/ -name "*.cpp" -o -name "*.h" -o -name "*.hpp")

if [ "$#" -ne 2 ]; then
  echo "Invalid number of args" >&2
  echo "Usage: clang-tidy.sh <compiler: gcc|clang> <build_mode: debug|release>" >&2
  exit 1
fi

compiler="$1"
build_mode="$2"

declare -A BUILD_DIRS=(
  ["gcc-debug"]="$GCC_DEBUG_BUILD_DIR"
  ["gcc-release"]="$GCC_RELEASE_BUILD_DIR"
  ["clang-debug"]="$CLANG_DEBUG_BUILD_DIR"
  ["clang-release"]="$CLANG_RELEASE_BUILD_DIR"
)

key="$compiler-$build_mode"

BUILD_DIR="${BUILD_DIRS[$key]}"

if [ -z "$BUILD_DIR" ]; then
    echo "Invalid compiler/build_mode combination: $key" >&2
    exit 1
fi

echo "Using build directory: $BUILD_DIR"
echo "Running clang-tidy..."

clang-tidy ${FILES} -p ${BUILD_DIR} --config-file=${CLANG_TIDY_CONFIG} --quiet
