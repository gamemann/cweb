#include "set.h"

/**
 * Copies a pointer from src to dst and frees dst if it is already allocated.
 * 
 * @param dst The destination string.
 * @param src The source string.
 * 
 * @return void
 */
void utils__ptr_set_str(char** dst, const char* src) {
    if (!dst)
        return;
    
    utils__ptr_free((void*)dst);

    *dst = strdup(src);
}

/**
 * Frees a pointer and sets it to NULL.
 * 
 * @param ptr The pointer to free.
 * 
 * @return void
 */
void utils__ptr_free(void** ptr) {
    if (ptr && *ptr) {
        free(*ptr);

        *ptr = NULL;
    }
}