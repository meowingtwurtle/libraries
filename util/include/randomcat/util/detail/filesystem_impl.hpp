// Internal header.
// Handles standard filesystem loading.
//
// Consumes macros:
// - RC_REQUIRE_FILESYSTEM - If defined, the program is ill-formed if standard filesystem is not loaded. Otherwise, ignored.
// - RC_DISABLE_FILESYSTEM - If defined, if standard filesystem was previously loaded, the program is ill-formed, otherwise standard filesystem will not be loaded even if found. Otherwise, ignored.
// Defines macros:
// - RC_HAVE_FILESYSTEM - 1 if standard filesystem was loaded, 0 otherwise.
// Defines program elements:
// - namespace randomcat::filesystem - If standard filesystem was loaded, namespace alias to standard filesystem. Otherwise, not defined.
// - namespace randomcat::fs - If standard filesystem was loaded, namespace alias to standard filesystem. Otherwise, not defined.

// Intentionally not pragma once.

#if defined(RC_DISABLE_FILESYSTEM)
#    ifdef RC_HAVE_FILESYSTEM
#        error Cannot retroactively disable loading of standard filesystem.
#    else
#        define RC_HAVE_FILESYSTEM 0

#        if RC_REQUIRE_FILESYSTEM
#            error STL filesystem is required but disabled.
#        endif
#    endif
#else
#    ifndef RC_HAVE_FILESYSTEM
#        if __cplusplus >= 2017
#            if __has_include(<filesystem>)
#                include <filesystem>

#                define RC_HAVE_FILESYSTEM 1
#                define RC_FS_NS ::std::filesystem
#            endif

#            if __has_include(<experimental/filesystem>)
#                include <experimental/filesystem>

#                define RC_HAVE_FILESYSTEM 1
#                define RC_FS_NS ::std::experimental::filesystem
#            endif
#        else
#            define RC_HAVE_FILESYSTEM 0
#        endif

#        if RC_HAVE_FILESYSTEM
namespace randomcat {
    namespace filesystem = RC_FS_NS;
    namespace fs = filesystem;
}    // namespace randomcat
#        endif
#    endif

#    if defined(RC_REQUIRE_FILESYSTEM) && (RC_HAVE_FILESYSTEM == 0)
#        error STL filesystem is required but not found.
#    endif
#endif

#undef RC_FS_NS
#undef RC_REQUIRE_FILESYSTEM
