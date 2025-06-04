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
// Path Manipulation Utility
// ----------------------------------------
// Cross-platform functions for resolving and joining file system paths.
// Provides:
//   - get_real_path(path)            – Returns a newly allocated, absolute canonical path
//   - get_real_path(path, buffer, n) – Writes the resolved path into a user buffer
//   - path_join(path1, path2)        – Concatenates two paths and returns a normalized absolute path
//
// Behavior:
//   - On POSIX: uses realpath(3) to resolve symlinks and “.”/“..” components.
//   - On Windows: uses GetFullPathNameA to obtain the absolute path.
//
// Memory Management:
//   - The two-argument get_real_path overload writes into a caller-provided buffer – no allocation.
//   - The one-argument get_real_path and path_join functions return a heap-allocated string.
//     Caller must free() the returned pointer.
//
// Dependencies:
//   - POSIX: <unistd.h> & realpath
//   - Windows: <windows.h> & GetFullPathNameA
//   - Fluent Lib C: string_builder.h for efficient string concatenation in path_join
//
// Example:
// ----------------------------------------
//   char *abs = get_real_path("./foo/bar.txt");
//   if (abs) { printf("Resolved: %s\n", abs); free(abs); }
//
//   char buf[512];
//   if (get_real_path("./foo/bar.txt", buf)) {
//       printf("Resolved in buffer: %s\n", buf);
//   }
//
//   char *joined = path_join("dir/sub", "file.txt");
//   if (joined) { printf("Joined: %s\n", joined); free(joined); }
//

// ============= FLUENT LIB C++ =============
#if defined(__cplusplus)
extern "C"
{
#endif

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
 * @brief Extracts the file name component from a given file system path.
 *
 * This function scans the input path and returns a newly allocated string containing
 * only the file name (the last segment after the final path separator).
 * If the path ends with a separator or is empty/NULL, NULL is returned.
 * The returned string must be freed by the caller.
 *
 * @param path The input file system path. Must not be NULL or empty.
 * @return A newly allocated string containing the file name, or NULL on error.
 */
static inline char *get_file_name(const char *const path)
{
    // Validate the input path
    if (!path || path[0] == '\0')
    {
        return NULL; // Invalid path
    }

    // Create a string builder to hold the file name
    string_builder_t sb;
    init_string_builder(&sb, 64, 1.5); // Initialize with a reasonable size

    // Iterate over the path to find the last separator
    for (size_t i = 0; path[i] != '\0'; i++)
    {
        // If we find a path separator, reset the string builder
        if (path[i] == PATH_SEPARATOR)
        {
            // Reset the string builder to start fresh for the next segment
            reset_string_builder(&sb);
        }
        else
        {
            // Append the character to the string builder
            write_char_string_builder(&sb, path[i]);
        }
    }

    // Return the collected file name from the string builder
    return collect_string_builder_no_copy(&sb);
}

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

/**
 * @brief Resolves the absolute, canonicalized path for a given file system path into a user-provided buffer.
 *
 * This function takes a file system path and writes its absolute, canonicalized version into the provided buffer.
 * On POSIX systems, it uses `realpath` to resolve symbolic links and relative components.
 * On Windows systems, it uses `GetFullPathNameA` to obtain the absolute path.
 *
 * @param path The input file system path to resolve. Must not be NULL or empty.
 * @param buffer The buffer to store the resolved absolute path. Must not be NULL.
 * @return 1 if the path was resolved successfully, 0 otherwise.
 */
static inline int get_real_path_buff(const char *const path, char *const buffer)
{
    // Validate the input path
    if (!path || path[0] == '\0')
    {
        return 0; // Invalid path
    }

    // Depending on the platform, use the appropriate method to resolve the real path
#ifndef _WIN32
    // Use realpath for POSIX systems
    char *resolved_path = realpath(path, buffer);
    if (!resolved_path)
    {
        return 0; // Failed to resolve the path
    }

    return resolved_path != NULL; // Return whether the path was resolved successfully
#else
    const size_t buf_size = sizeof(buffer);
    DWORD len = GetFullPathNameA(path, (DWORD)buf_size, buffer, NULL);
    return len > 0 && len < buf_size ? 1 : 0; // Return whether the path was resolved successfully
#endif
}

/**
 * @brief Joins two file system paths and returns the normalized absolute path.
 *
 * This function concatenates two paths using the appropriate path separator for the platform,
 * then normalizes the result to resolve any redundant separators, relative components, or symbolic links.
 * The returned path is an absolute, canonicalized path.
 *
 * @param path1 The first path component. Must not be NULL or empty.
 * @param path2 The second path component. Must not be NULL or empty.
 * @return A newly allocated string containing the normalized absolute path,
 *         or NULL if the input is invalid, the path cannot be resolved,
 *         or memory allocation fails. The caller is responsible for freeing the returned string.
 */
static inline char *path_join(const char *const path1, const char *const path2)
{
    // Validate the input paths
    if (!path1 || !path2 || path1[0] == '\0' || path2[0] == '\0')
    {
        return NULL; // Invalid paths
    }

    // Use a string builder to construct the joined path
    string_builder_t sb;
    init_string_builder(&sb, 256, 1.5); // Initialize with a reasonable size
    write_string_builder(&sb, path1); // Write the first path
    write_char_string_builder(&sb, PATH_SEPARATOR); // Add the path separator
    write_string_builder(&sb, path2); // Write the second path

    // Collect the result from the string builder without copying
    const char *joined_path = collect_string_builder_no_copy(&sb);

    // Normalize the path to remove any redundant separators
    char *normalized_path = get_real_path(joined_path);

    // Free the string builder resources
    destroy_string_builder(&sb);

    // Return the normalized path
    return normalized_path;
}

// ============= FLUENT LIB C++ =============
#if defined(__cplusplus)
}
#endif

#endif //FLUENT_LIBC_PATH_LIBRARY_H