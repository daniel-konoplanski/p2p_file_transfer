#!/bin/bash

cmake -S . -B build --preset x86_64-linux-gcc
cmake --build build
