#!/usr/bin/env bash

# Stop if an error occurs
set -e

# Variables
# > Check if a custom CMake generator is wanted
if [ -n "$STAMMBAUM_CMAKE_GENERATOR" ]; then
    echo "> Found custom \$STAMMBAUM_CMAKE_GENERATOR, use '$STAMMBAUM_CMAKE_GENERATOR' as CMake generator"
else
    if [[ "$OSTYPE" == "linux-gnu" ]] || [[ "$OSTYPE" == "linux-ms" ]]; then
        STAMMBAUM_CMAKE_GENERATOR="Unix Makefiles"
    elif [[ "$OSTYPE" == "msys" ]]; then
        STAMMBAUM_CMAKE_GENERATOR="MinGW Makefiles"
    else
        echo "Platform not supported! ($OSTYPE) [set \$STAMMBAUM_CMAKE_GENERATOR to continue]" >&2
        exit 1
    fi
    echo "> \$STAMMBAUM_CMAKE_GENERATOR not set, use default: '$STAMMBAUM_CMAKE_GENERATOR'"
    export STAMMBAUM_CMAKE_GENERATOR
fi
# > Check if CMake scripts should be executed with a custom thread number
if [[ -n "$STAMMBAUM_CMAKE_BUILD_THREAD_COUNT" ]]; then
    echo "> Found custom \$STAMMBAUM_CMAKE_BUILD_THREAD_COUNT, use '$STAMMBAUM_CMAKE_BUILD_THREAD_COUNT' as CMake build thread count"
else
    if [[ "$OSTYPE" == "linux-gnu" ]] || [[ "$OSTYPE" == "linux-ms" ]]; then
        STAMMBAUM_CMAKE_BUILD_THREAD_COUNT=$(cat /proc/cpuinfo | grep -c processor)
    elif [[ "$OSTYPE" == "msys" ]]; then
        STAMMBAUM_CMAKE_BUILD_THREAD_COUNT=$NUMBER_OF_PROCESSORS
    else
        STAMMBAUM_CMAKE_BUILD_THREAD_COUNT=4
    fi
    echo "> \$STAMMBAUM_CMAKE_BUILD_THREAD_COUNT not set, use default: '$STAMMBAUM_CMAKE_BUILD_THREAD_COUNT'"
    export STAMMBAUM_CMAKE_BUILD_THREAD_COUNT
fi
# > Check if custom CMake configure flags are wanted
if [[ -n "$STAMMBAUM_CMAKE_CONFIGURE_FLAGS" ]]; then
    echo "> Found custom \$STAMMBAUM_CMAKE_CONFIGURE_FLAGS, use '$STAMMBAUM_CMAKE_CONFIGURE_FLAGS' as CMake configure flags"
else
    DEFAULT_CROSS_PLATFORM_CMAKE_BUILD_FLAGS="-Wdev -Wdeprecated"
    if [[ "$OSTYPE" == "linux-gnu" ]] || [[ "$OSTYPE" == "linux-ms" ]]; then
        STAMMBAUM_CMAKE_CONFIGURE_FLAGS="$DEFAULT_CROSS_PLATFORM_CMAKE_BUILD_FLAGS -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"
    elif [[ "$OSTYPE" == "msys" ]]; then
        STAMMBAUM_CMAKE_CONFIGURE_FLAGS="$DEFAULT_CROSS_PLATFORM_CMAKE_BUILD_FLAGS -DCMAKE_SH=CMAKE_SH-NOTFOUND"
    else
        echo "Platform not supported! ($OSTYPE) [set variable to continue]" >&2
        exit 1
    fi
    echo "> \$STAMMBAUM_CMAKE_CONFIGURE_FLAGS not set, use default: '$STAMMBAUM_CMAKE_CONFIGURE_FLAGS'"
    export STAMMBAUM_CMAKE_CONFIGURE_FLAGS
fi
# > Check if custom CMake build flags are wanted
# Check if additional CMake "run build" flags are wanted
if [[ -n "$STAMMBAUM_CMAKE_BUILD_FLAGS" ]]; then
    echo "> Found custom \$STAMMBAUM_CMAKE_BUILD_FLAGS, use '$STAMMBAUM_CMAKE_BUILD_FLAGS' as CMake build flags"
else
    STAMMBAUM_CMAKE_BUILD_FLAGS="-j $STAMMBAUM_CMAKE_BUILD_THREAD_COUNT --no-print-directory"
    echo "> \$CMAKE_RUN_BUILD_FLAGS_ADDITIONAL not set, use default: '$CMAKE_RUN_BUILD_FLAGS_ADDITIONAL'"
    export STAMMBAUM_CMAKE_BUILD_FLAGS
fi
# > Check if a custom CMake build type is wanted
if [[ -n "$STAMMBAUM_CMAKE_BUILD_TYPE" ]]; then
    echo "> Found custom \$STAMMBAUM_CMAKE_BUILD_TYPE, use '$STAMMBAUM_CMAKE_BUILD_TYPE' as CMake build type"
else
    STAMMBAUM_CMAKE_BUILD_TYPE=Release
    echo "> \$STAMMBAUM_CMAKE_BUILD_TYPE not set, use default: '$STAMMBAUM_CMAKE_BUILD_TYPE'"
    export STAMMBAUM_CMAKE_BUILD_TYPE
fi

mkdir -p build
cd build

# Activate debugging to show the generated commands
set -x

# Run CMake to create build files
cmake .. -G "$STAMMBAUM_CMAKE_GENERATOR" $STAMMBAUM_CMAKE_CONFIGURE_FLAGS -DCMAKE_BUILD_TYPE="$STAMMBAUM_CMAKE_BUILD_TYPE"
LOCAL_BUILD_STATUS=$?

# Disable debugging
{ set +x; } 2>/dev/null

if ! [[ $LOCAL_BUILD_STATUS -eq 0 ]]; then
    echo -e "\e[1;31mError: CMake could not generate build files!\e[0m" >&2
    exit 1
fi

# Activate debugging to show the generated commands
set -x

# Run "make" or something else that is generator dependent
cmake --build . --config "$STAMMBAUM_CMAKE_BUILD_TYPE" -- $STAMMBAUM_CMAKE_BUILD_FLAGS
LOCAL_BUILD_TARGET_STATUS=$?

# Disable debugging
{ set +x; } 2>/dev/null

if ! [[ $LOCAL_BUILD_TARGET_STATUS -eq 0 ]]; then
    echo -e "\e[1;31mError: CMake could not build target '$LOCAL_CMAKE_BUILD_TARGET'!\e[0m" >&2
    exit 1
fi
