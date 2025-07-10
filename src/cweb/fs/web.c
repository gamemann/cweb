#include "web.h"

char* fs__web_get_html(char* uri, const char* fs_root) {
    if (!uri || uri[0] != '/')
        return NULL;

    char path[MAX_FILE_LEN];

    // Check for index (/).
    if (strcmp(uri, "/") == 0) {
        snprintf(path, sizeof(path), "%s/index.html", fs_root);

        goto try_read;
    }

    // Copy URI.
    char *uri_cpy = strdup(uri);

    if (!uri_cpy)
        return NULL;

    // Check for '..' (next top folder) and reject it.
    if (strstr(uri_cpy, "..")) {
        free(uri_cpy);

        return NULL;
    }

    char *rel = uri_cpy + 1;

    // Try direct .html match.
    snprintf(path, sizeof(path), "%s/%s.html", fs_root, rel);

    if (utils__file_exists(path)) {
        free(uri_cpy);

        goto try_read;
    }

    // Try path/index.html.
    snprintf(path, sizeof(path), "%s/%s/index.html", fs_root, rel);

    if (utils__file_exists(path)) {
        free(uri_cpy);

        goto try_read;
    }

    // Free path copy.
    free(uri_cpy);

    return NULL;

try_read:;
    // Read contents of file if possible.
    char *buffer = NULL;

    if (utils__read_file(path, &buffer) != 0)
        return NULL;

    return buffer;
}