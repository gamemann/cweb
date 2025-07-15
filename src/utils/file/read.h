#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#include <ctype.h>

int utils__read_file(const char* path, char** buffer);
int utils__file_exists(const char* path);