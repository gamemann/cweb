#include "web.h"

/**
 * Retrieve contents from the custom HTML file system we have.
 * 
 * @param uri The URI/path of the HTTP request.
 * @param fs_root The path to the directory that acts as a HTML file system.
 * @param buffer The buffer to store the HTML contents in.
 * 
 * @return 0 on success or 1 on error.
 */
int fs__web_get_html(char* uri, const char* fs_root, char** buffer) {
    if (!uri || uri[0] != '/') {
        ERR_SET(1, "Invalid path (NULL or doesn't start with '/').");

        return 1;
    }

    char path[MAX_FILE_LEN];

    // Check for index (/).
    if (strcmp(uri, "/") == 0) {
        snprintf(path, sizeof(path), "%s/index.html", fs_root);

        goto try_read;
    }

    // Copy URI.
    char *uri_cpy = strdup(uri);

    if (!uri_cpy) {
        ERR_SET(2, "Failed to copy URL.");

        return 1;
    }

    // Check for '..' (next top folder) and reject it.
    if (strstr(uri_cpy, "..")) {
        free(uri_cpy);

        ERR_SET(3, "Found '..' (directory up). Somebody may be trying to exploit the file system!");

        return 1;
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

    return 0;

try_read:;
    // Read contents of file if possible.

    if (utils__read_file(path, buffer) != 0)
        return 1;

    return 0;
}