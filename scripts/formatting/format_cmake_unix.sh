#!/usr/bin/env bash

# Prerequisites:
# - gersemi (pip3 install gersemi)
#   [https://github.com/BlankSpruce/gersemi]

# CLI args:
# > help:
if [[ "$*" == *"--help"* ]]; then
    echo "USAGE: ./format_cmake_unix.sh [OPTIONS]

OPTIONS:
    --check <returns error if a file not already correctly formatted>
    --dry-run <show what will change but do not update files>
    --help <help>
    --version <version>"
    exit 0
fi
# > version:
if [[ "$*" == *"--version"* ]]; then
    echo "1.0.0"
    exit 0
fi
# > options:
if [[ "$*" == *"--dry-run"* ]]; then
    FORMAT_FILES_DRY_RUN=true
fi
if [[ "$*" == *"--check"* ]]; then
    FORMAT_FILES_CHECK=true
fi

# ------------------------------------------------------------------------
# >>> Main
# ------------------------------------------------------------------------

EXIT_VALUE=0

CMAKELISTS_FILES=$(find . -regex ".*/CMakeLists.txt" \
            ! -path '*/vendor/*' ! -path '*/build*/*' \
            -print0 | xargs -n 1 -0 echo)

CMAKELISTS_FILES_ARE_CORRECTLY_FORMATTED=true
CMAKELISTS_FILES_THAT_ARE_INCORRECTLY_FORMATTED=()
CMAKELISTS_GERSEMI_ARGS=()

if [[ "$FORMAT_FILES_CHECK" = true ]]; then
    CMAKELISTS_GERSEMI_ARGS+=(--check)
else
    # Enable dry run only if the files are not checked
    if [[ "$FORMAT_FILES_DRY_RUN" = true ]]; then
        CMAKELISTS_GERSEMI_ARGS+=(--diff)
    else
        CMAKELISTS_GERSEMI_ARGS+=(--in-place)
    fi
fi

IFS=$'\n'
for CMAKELISTS_FILE in $CMAKELISTS_FILES; do
    echo "> gersemi $CMAKELISTS_GERSEMI_ARGS $CMAKELISTS_FILE"
    gersemi $CMAKELISTS_GERSEMI_ARGS "$CMAKELISTS_FILE"
    if [[ $? -eq 0 ]]; then
        if [[ "$FORMAT_FILES_CHECK" = true ]]; then
            echo "  ALREADY CORRECTLY FORMATTED"
        fi
    else
        if [[ "$FORMAT_FILES_CHECK" = true ]]; then
            echo "  [ERROR] NOT ALREADY CORRECTLY FORMATTED"
        fi
        CMAKELISTS_FILES_ARE_CORRECTLY_FORMATTED=false
        CMAKELISTS_FILES_THAT_ARE_INCORRECTLY_FORMATTED+=("$CMAKELISTS_FILE")
    fi
done

if [[ "$FORMAT_FILES_CHECK" = true ]]; then
    if [[ "$CMAKELISTS_FILES_ARE_CORRECTLY_FORMATTED" = true ]]; then
        echo "All CMakeLists.txt files are properly formatted."
    else
        echo "The following CMakeLists.txt files are not properly formatted:"
        for CMAKELISTS_FILE_THAT_IS_INCORRECTLY_FORMATTED in "${CMAKELISTS_FILES_THAT_ARE_INCORRECTLY_FORMATTED[@]}"; do
            echo "- ${CMAKELISTS_FILE_THAT_IS_INCORRECTLY_FORMATTED}"
        done
        EXIT_VALUE=1
    fi
fi

# ------------------------------------------------------------------------
# >>> Exit
# ------------------------------------------------------------------------

exit $EXIT_VALUE
