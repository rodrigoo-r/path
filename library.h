/*
 * This code is distributed under the terms of the GNU General Public License.
 * For more information, please refer to the LICENSE file in the root directory.
 * -------------------------------------------------
 * Copyright (C) 2025 Rodrigo R.
 * This program comes with ABSOLUTELY NO WARRANTY; for details type show w'.
 * This is free software, and you are welcome to redistribute it
 * under certain conditions; type show c' for details.
*/

#ifndef FLUENT_LIBC_PATH_LIBRARY_H
#define FLUENT_LIBC_PATH_LIBRARY_H

// ============= FLUENT LIB C =============

// ============= INCLUDES =============
#ifndef _WIN32
#   include <unistd.h> // For POSIX path functions
#   define PATH_SEPARATOR '/'
#else
#   include <windows.h> // For Windows path functions
#   define PATH_SEPARATOR '\\'
#endif
#ifndef FLUENT_LIBC_RELEASE
#   include <string_builder.h> // fluent_libc
#else
#   include <fluent/string_builder/string_builder.h> // fluent_libc
#endif

/**
 * @brief Resolves the absolute, canonicalized path for a given file system path.
 *
 * This function takes a file system path and returns its absolute, canonicalized version.
 * On POSIX systems, it uses `realpath` to resolve symbolic links, relative components,
 * and returns a newly allocated string containing the resolved path.
 * On Windows systems, it uses `GetFullPathNameA` to obtain the absolute path.
 *
 * @param path The input file system path to resolve. Must not be NULL or empty.
 * @return A newly allocated string containing the resolved absolute path,
 *         or NULL if the input is invalid, the path cannot be resolved,
 *         or memory allocation fails. The caller is responsible for freeing the returned string.
 */
static inline char *get_real_path(const char *const path)
{
    // Validate the input path
    if (!path || path[0] == '\0')
    {
        return NULL; // Invalid path
    }

    // Depending on the platform, use the appropriate method to resolve the real path
#ifndef _WIN32
    // Use realpath for POSIX systems
    char *resolved_path = realpath(path, NULL);
    if (!resolved_path)
    {
        return NULL; // Failed to resolve the path
    }

    return resolved_path; // Return the resolved path
#else
    // Warning: Reprocesses the same path, no way around it
    // without overallocating for Windows systems
    // Use GetFullPathName for Windows systems
    DWORD length = GetFullPathNameA(path, 0, NULL, NULL);
    if (length == 0)
    {
        return NULL; // Failed to get the path length
    }

    char *resolved_path = (char *)malloc(length + 1);
    if (!resolved_path)
    {
        return NULL; // Memory allocation failed
    }

    GetFullPathNameA(path, length + 1, resolved_path, NULL);
    return resolved_path; // Return the resolved path
#endif
}

#endif //FLUENT_LIBC_PATH_LIBRARY_H