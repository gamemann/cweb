#include "helpers.h"

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

    cnt += last < (str + strlen(str) - 1) + 1;

    res = malloc(sizeof(char*) * cnt);

    if (!res)
        return res;

    size_t idx = 0;
    char *token = strtok(str, f_delim);

    while (token) {
        if (idx > cnt)
            break;

        *(res + idx++) = strdup(token);

        *(res + idx) = NULL;
    }

    return res;
}

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