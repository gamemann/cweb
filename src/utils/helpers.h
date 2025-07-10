#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

char** utils__str_split(char* str, const char delim);
int utils__get_delim_cnt(char* str, const char delim);
int utils__read_file(const char* path, char** buffer);
int utils__file_exists(const char* path);