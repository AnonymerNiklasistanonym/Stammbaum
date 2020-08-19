#!/usr/bin/env bash

# Prerequisites:
# - astyle
#   [http://astyle.sourceforge.net/]

# CLI args:
# > help:
if [[ "$*" == *"--help"* ]]; then
    echo "USAGE: ./format_code_cpp_unix.sh [OPTIONS]

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

CODE_CPP_ASTYLE_ASTYLERC="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )/astyle_config.astylerc"
#CODE_CPP_ASTYLE_ARGS=(--project="$CODE_CPP_ASTYLE_ASTYLERC")
CODE_CPP_ASTYLE_ARGS=(
    --style=kr
    --indent=spaces=4
    --align-pointer=name
    --align-reference=name
    --convert-tabs
    --attach-namespaces
    --max-code-length=100
    --max-instatement-indent=120
    --pad-header
    --pad-oper
)


if [[ "$FORMAT_FILES_CHECK" = true ]]; then
    CODE_CPP_ASTYLE_ARGS+=(--dry-run --formatted)
else
    # Enable dry run only if the files are not checked
    if [[ "$FORMAT_FILES_DRY_RUN" = true ]]; then
        CODE_CPP_ASTYLE_ARGS+=(--dry-run)
    fi
fi

OUTPUT=$( find ./ -regex ".*\.\(c\|cpp\|hpp\|h\)" \
            ! -path '*/vendor/*' ! -path '*/build*/*' \
            -print0 | xargs --null -n 1 -0 astyle "${CODE_CPP_ASTYLE_ARGS[@]}")

if [[ "$FORMAT_FILES_CHECK" = true ]]; then
    if [[ -z "$OUTPUT" ]]; then
        echo "All C++ source code files are properly formatted."
        exit 0
    else
        echo "Some C++ source code files are not properly formatted:"
        echo "${OUTPUT}" \
             | sed -e "s/Formatted/> Bad format  /" \
             | sed -e "s/Unchanged/Correct format/"
        EXIT_VALUE=1
    fi
else
    if [[ "$FORMAT_FILES_DRY_RUN" = true ]]; then
        echo "${OUTPUT}" \
             | sed -e "s/Formatted/> Bad format  /" \
             | sed -e "s/Unchanged/Correct format/"
    fi
fi

exit $EXIT_VALUE
