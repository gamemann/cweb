#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/error/error.h>

#include <utils/file/read.h>

int fs__web_get_html(char* uri, const char* fs_root, char** buffer);