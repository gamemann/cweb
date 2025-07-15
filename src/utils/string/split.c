#include "split.h"

/**
 * Splits a string by a delim.
 * 
 * @param str The string you'd like to split.
 * @param delim The delim to split by.
 * 
 * @return A pointer to pointer to the results or NULL on error.
 */
char** utils__str_split(char* str, const char delim) {
    char **res = NULL;
    size_t cnt = 0;
    char *tmp = str;

    char *last = NULL;

    char f_delim[2];
    f_delim[0] = delim;
    f_delim[1] = 0;

    while (*tmp) {
        if (*tmp == delim) {
            last = tmp;
            cnt++;
        }

        tmp++;
    }

    if (str[0] != '\0')
        cnt++;

    res = malloc(sizeof(char*) * (cnt + 1));

    if (!res)
        return res;

    size_t idx = 0;

    char *save_ptr;
    char *token = strtok_r(str, f_delim, &save_ptr);

    while (token && idx < cnt) {
        res[idx++] = strdup(token);
        token = strtok_r(NULL, f_delim, &save_ptr);
    }

    res[idx] = NULL;

    return res;
}

/**
 * Retrieves the delim count.
 * 
 * @param str The string to search in.
 * @param delim The delim to search for.
 * 
 * @return The amount of occurrences.
 */
int utils__get_delim_cnt(char* str, const char delim) {
    int cnt = 0;

    char *tmp = str;

    while (*tmp) {
        if (*tmp == delim)
            cnt++;

        tmp++;
    }

    return cnt;
}