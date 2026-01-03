# runs clang-format
# clang-format.sh <>

#!bin/bash

FILES=$(find src/ -name "*.cpp" -o -name "*.h" -o -name "*.hpp")

echo "Running clang-format..."
echo "Found files: ${FILES}"

clang-format -i --style=file ${FILES}
