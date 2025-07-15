#include "read.h"

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