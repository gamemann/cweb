#pragma once

#include <stdlib.h>
#include <string.h>

#include <utils/constants.h>
#include <utils/int_types.h>

void utils__ptr_set_str(char** dst, const char* src);
void utils__ptr_free(void** ptr);