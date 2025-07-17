#include "error.h"

__thread error_ctx_t err_ctx = {0};

/**
 * Sets a custom error.
 * 
 * @param code The error code.
 * @param func The name of the function the error occured in.
 * @param msg The error message with argument support.
 * 
 * @return void
 */
void utils__error_set(int code, const char* func, const char* msg, ...) {
    err_ctx.code = code;

    utils__str_copy(err_ctx.func, func, sizeof(err_ctx.func));

    // Format message and assign to context message.
    va_list args;
    va_start(args, msg);

    vsnprintf(err_ctx.msg, sizeof(err_ctx.msg), msg, args);
    
    va_end(args);
}

/**
 * Retrieves the current error context.
 * 
 * @return A pointer to the current error context declared in the global scope.
 */
error_ctx_t* utils__error_ctx() {
    return &err_ctx;
}