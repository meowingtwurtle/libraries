// Loads standard filesystem, if possible.
//
// Consumes macros:
// - RC_DISABLE_FILESYSTEM - If defined, if standard filesystem was previously loaded, the program is ill-formed, otherwise standard filesystem will not be loaded even if found. Otherwise, ignored.
// Defines macros:
// - RC_HAVE_FILESYSTEM - 1 if standard filesystem was loaded, 0 otherwise.
// Defines program elements:
// - namespace randomcat::filesystem - If standard filesystem was loaded, namespace alias to standard filesystem. Otherwise, not defined.
// - namespace randomcat::fs - If standard filesystem was loaded, namespace alias to standard filesystem. Otherwise, not defined.

// Intentionally not pragma once.

#include "randomcat/util/detail/filesystem_impl.hpp"
