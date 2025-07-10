#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <cweb/ctx/ctx.h>

#ifdef CONF_USE_FACILIO
#include <cweb/server/facilio.h>
#else
#include <cweb/server/raw.h>
#endif

int server__setup(ctx_t* ctx);