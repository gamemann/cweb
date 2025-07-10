#include "server.h"

int server__setup(ctx_t* ctx) {
#ifdef CONF_USE_FACILIO
    return server__setup_facilio(ctx);
#else
    return server__setup_raw(ctx);
#endif
}