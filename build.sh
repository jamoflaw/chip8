#!/bin/sh

set -x 

# Performance Optimisations
MEMDEBUG=0
DEBUGSYMBOLS=1
VERBOSEWARNINGS=1

# Build Type
BUILD_TYPE=Debug
#BUILD_TYPE=Release

# Run cmake to create the build scripts
cmake -H. -B.cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DDEBUGSYMBOLS=$DEBUGSYMBOLS -DMEMDEBUG=$MEMDEBUG -DVERBOSEWARNINGS=$VERBOSEWARNINGS

MAKEOPTS="-j$(nproc --ignore=1)"

# Build
cd .cmake
make $MAKEOPTS Chip8

exit 0
 
