#include "server.h"

/**
 * Sets up the HTTP server.
 * 
 * @param ctx A pointer to the context.
 * 
 * @return 0 on success or value of server mode setup.
 */
int server__setup(ctx_t* ctx) {
#ifdef CONF_USE_FACILIO
    return server__setup_facilio(ctx);
#else
    return server__setup_raw(ctx);
#endif
}