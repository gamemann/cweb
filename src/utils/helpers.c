#include "helpers.h"

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

/**
 * Reads the contents of a file and allocates/stores it in buffer.
 * 
 * @param path The path/file to read.
 * @param buffer A pointer to pointer of the buffer.
 * 
 * @return 0 on success. 1 on fopen() error. 2 on invalid file size. 3 on buffer allocation error.
 */
int utils__read_file(const char* path, char** buffer) {
    FILE *fp = fopen(path, "r");

    if (!fp)
        return 1;

    fseek(fp, 0, SEEK_END);
    ssize_t file_sz = ftell(fp);
    rewind(fp);

    if (file_sz <= 0) {
        fclose(fp);

        return 2;
    }

    *buffer = malloc(file_sz + 1);

    if (*buffer == NULL) {
        fclose(fp);

        return 3;
    }

    size_t r = fread(*buffer, 1, file_sz, fp);

    (*buffer)[r] = '\0';

    fclose(fp);

    return 0;
}

/**
 * Checks if a file exists.
 * 
 * @param path The path to check.
 * 
 * @return 1 on yes or 0 on no.
 */
int utils__file_exists(const char* path) {
    FILE *fp = fopen(path, "r");

    if (fp) {
        fclose(fp);

        return 1;
    }

    return 0;
}

char* utils__trim(char *str) {
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