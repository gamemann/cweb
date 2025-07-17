#include "logger.h"

/**
 * Retrieves log level string.
 * 
 * @param lvl The log level (enum).
 * 
 * @return A string (const char*) of the level.
 */
const char* logger__get_lvl_str(log_level_t lvl) {
    switch (lvl) {
        case LVL_FATAL:
            return "FATAL";

        case LVL_ERROR:
            return "ERROR";

        case LVL_WARN:
            return "WARN";

        case LVL_NOTICE:
            return "NOTICE";

        case LVL_INFO:
            return "INFO";

        case LVL_DEBUG:
            return "DEBUG";

        case LVL_TRACE:
            return "TRACE";
    }

    // Shouldn't get here...
    return "N/A";
}

/**
 * Prints a raw log message.
 * 
 * @param lvl The current level.
 * @param req_lvl The required level.
 * @param log_path The path to log the message to (NULL = no logging to file).
 * @param msg The full log message.
 * @param args Arguments for log message.
 * 
 * @return 0 on success or 1 on error.
 */
int logger__log_raw(log_level_t lvl, log_level_t req_lvl, const char* log_path, const char* msg, va_list args) {
    if ((int)req_lvl > (int)lvl)
        return 0;

    FILE *pipe = stdout;

    // Check for errors.
    if (lvl == LVL_FATAL || lvl == LVL_ERROR)
        pipe = stderr;

    // Handle message format.
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(NULL, 0, msg, args_copy);
    va_end(args_copy);

    if (len < 0) {
        ERR_SET(1, "Message length after message format is below 0.");

        return 1;
    }

    // Format log message.
    char f_msg[len + 1];
    vsnprintf(f_msg, sizeof(f_msg), msg, args);

    // Retrieve level string.
    const char *lvl_str = logger__get_lvl_str(req_lvl);

    // Format log full message.
    char full_msg[len + MAX_NAME_LEN + 1];
    snprintf(full_msg, sizeof(full_msg), "[%s] %s", lvl_str, f_msg);

    // Print main log message.
    fprintf(pipe, "%s\n", full_msg);

    // Check for log path.
    if (log_path) {
        // Open log file.
        FILE *log_file = fopen(log_path, "a");

        if (!log_file) {
            ERR_SET(2, "Failed to open log path '%s': %s (%d)", log_path, strerror(errno), errno);

            return 1;
        }

        // Retrieve time.
        time_t now = time(NULL);
        struct tm *tm_val = localtime(&now);

        if (!tm_val) {
            fclose(log_file);

            ERR_SET(3, "Failed to allocate tm_val.");

            return 1;
        }

        // Format full log message to file.
        char log_file_msg[len + MAX_NAME_LEN + MAX_DATE_LEN + 1];

        snprintf(log_file_msg, sizeof(log_file_msg), "[%02d-%02d-%02d %02d:%02d:%02d]%s", tm_val->tm_year % 100, tm_val->tm_mon + 1, tm_val->tm_mday,
        tm_val->tm_hour, tm_val->tm_min, tm_val->tm_sec, full_msg);

        // Write to log file and close.
        fprintf(log_file, "%s\n", log_file_msg);

        fclose(log_file);
    }

    return 0;
}

/**
 * Logs a message using the config structure directly.
 * 
 * @param cfg A pointer to the config.
 * @param req_lvl The required level for the log message.
 * @param msg The full log message.
 * @param args Arguments for the log message.
 * 
 * @return void
 */
void logger__log(config_t* cfg, log_level_t req_lvl, const char* msg, ...) {
    va_list args;
    va_start(args, msg);

    logger__log_raw((log_level_t) cfg->log_lvl, req_lvl, cfg->log_file, msg, args);

    va_end(args);
}