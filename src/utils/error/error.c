#include "error.h"

error_ctx_t err_ctx = {0};

void utils__error_set(int code, const char* func, const char* msg, ...) {
    err_ctx.code = code;

    utils__str_copy(err_ctx.func, func, sizeof(err_ctx.func));

    // Format message and assign to context message.
    va_list args;
    va_start(args, msg);

    vsnprintf(err_ctx.msg, sizeof(err_ctx.msg), msg, args);
    
    va_end(args);
}

error_ctx_t* utils__error_ctx() {
    return &err_ctx;
}