#pragma once

// C++20 format library is not yet implemented in most compilers
#if __has_include(<format>)
#include <format>
namespace format = std;
#else
// If not use fmt as replacement which has the same implementation for format:
// (https://github.com/fmtlib/fmt)
#include <fmt/core.h>
namespace format = fmt;
#endif
