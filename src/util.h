#pragma once

#include <stdint.h>
#include <stdio.h>

void Logf(FILE* fp, const char* file, int32_t line, const char* func, const char* format, ...);

#define ERRORF(...) Logf(stderr, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define INFOF(...) Logf(stdout, __FILE__, __LINE__, __func__, __VA_ARGS__)
