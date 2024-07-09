#!/bin/bash
chmod +x clean.sh
rm -rf build
rm -rf GaitSymQt/build

if [ "$1" = "-f" ]; then
    rm GaitSymQt/CMakeLists.txt.user
fi
