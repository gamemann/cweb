#include "trim.h"

/**
 * Trims a string.
 * 
 * @param str The string to trim.
 * 
 * @return The trimmed string.
 */
char* utils__str_trim(char *str) {
    if (!str)
        return NULL;

    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0')
        return str;

    char *end = str + strlen(str) - 1;

    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    *(end + 1) = '\0';

    return str;
}