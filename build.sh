#!/bin/sh

set -x 

# Performance Optimisations
EVO_MEMDEBUG=1
EVO_DEBUGSYMBOLS=1
EVO_VERBOSEWARNINGS=1

# Build Type
BUILD_TYPE=Debug
#BUILD_TYPE=Release

# Run cmake to create the build scripts
cmake -H. -B.cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DEVO_DEBUGSYMBOLS=$EVO_DEBUGSYMBOLS -DEVO_MEMDEBUG=$EVO_MEMDEBUG -DEVO_VERBOSEWARNINGS=$EVO_VERBOSEWARNINGS

MAKEOPTS="-j$(nproc --ignore=1)"

# Build
cd .cmake
make $MAKEOPTS Chip8

exit 0
# For stats run cloc
if hash cloc 2>/dev/null; then
  STATS=$(cloc --match-d '(src|inc)' ..)

  TIMESTAMP=$(date +%Y%m%d-%H%M%S)
  CODE_COMMENTS=$(echo "$STATS" | grep -e '^C++' | awk '{print $4;}')
  CODE_LINES=$(echo "$STATS" | grep -e '^C++' | awk '{print $5;}')
  HEADER_COMMENTS=$(echo "$STATS" | grep -e '^C/' | awk '{print $4;}')
  HEADER_LINES=$(echo "$STATS" | grep -e '^C/' | awk '{print $5;}')

  echo $TIMESTAMP,$HEADER_LINES,$HEADER_COMMENTS,$CODE_LINES,$CODE_COMMENTS >> ../clocstats.txt
fi
 