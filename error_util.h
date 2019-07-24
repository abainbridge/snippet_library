#pragma once

#include <stdio.h>
#include <stdlib.h>

#define FATAL_ERROR(msg, ...) { fprintf(stderr, msg "\n", ##__VA_ARGS__); exit(-1); }
