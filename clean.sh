#!/bin/bash
echo "Delete build leftovers. Use -f to delete the CMakeLists.txt.user files."
chmod +x clean.sh
rm -rf build
rm -rf GaitSymQt/build
rm -rf terminal/build

if [ "$1" = "-f" ]; then
    rm -vf GaitSymQt/CMakeLists.txt.user
fi
