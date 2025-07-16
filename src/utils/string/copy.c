#include "copy.h"

/**
 * Safely copies a source string to a destination. Uses strncpy() and automatically handles null terminator.
 * 
 * @param dst The destination string.
 * @param src The source string.
 * @param sz The maximum size of the destination string.
 * 
 * @return void
 */
static inline void utils__str_copy(char* dst, const char* src, size_t sz) {
    strncpy(dst, src, sz - 1);
    dst[sz - 1] = '\0';
}