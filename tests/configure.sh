#!/usr/bin/env bash

out="../out/tests/build"

cmake -S . -B $out -G "Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_PREFIX_PATH="/d/packages/Catch2/lib/cmake/Catch2"