// Loads standard filesystem, causing the program to be ill-formed if it could not be loaded.
//
// Consumes macros:
// - RC_DISABLE_FILESYSTEM - If defined, the program is ill-formed. Otherwise, ignored.
// Defines macros:
// - RC_HAVE_FILESYSTEM - Defined to be 1.
// Defines program elements:
// - namespace randomcat::filesystem - Namespace alias to standard filesystem.
// - namespace randomcat::fs - Namespace alias to standard filesystem.

// Intentionally not pragma once.

#define RC_REQUIRE_FILESYSTEM
#include "randomcat/util/detail/filesystem_impl.hpp"
