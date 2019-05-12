#pragma once

#include <stdio.h>
#include "Status.hpp"

StatusCode init_debug();

void debug(const char* string);

void info(const char* string);

void info(const char* string, StatusCode status);

void abort(const char *msg, const char *file, int line);




