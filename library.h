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

#endif //FLUENT_LIBC_PATH_LIBRARY_H