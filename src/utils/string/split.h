#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#include <ctype.h>

char** utils__str_split(char* str, const char delim);
int utils__get_delim_cnt(char* str, const char delim);