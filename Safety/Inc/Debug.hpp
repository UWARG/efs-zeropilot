#pragma once

#include <stdio.h>
#include "Status.hpp"

StatusCode init_debug();

void debug(const char* string);

void info(const char* string);

void error(const char* string);

void error(const char* string, uint32_t code);

void info(const char* string, StatusCode status);

/**
 * @param string Will be prepended into message
 * @param data
 * @param data_size
 * @param display_char Wether to display ASCII values or HEX values
 */
void debug_array(const char* string, uint8_t* data, size_t data_size, bool display_char = false);

void abort(const char *msg, const char *file, int line);




