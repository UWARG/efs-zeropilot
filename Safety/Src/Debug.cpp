#include <stdio.h>
#include "Debug.hpp"
#include "UART.hpp"
#include "Status.hpp"
#include <string.h>

static const UARTPortNum DEBUG_UART_PORT = UART_PORT1;
static const uint32_t DEBUG_BAUDRATE = 115200;

UARTPort port;
bool port_setup = false;

static char buffer[512];

StatusCode init_debug(){
	UARTSettings settings;
	settings.baudrate = DEBUG_BAUDRATE;
	settings.cts_rts = false;
	settings.parity = UART_NO_PARITY;
	settings.stop_bits = 1;

	port = UARTPort(DEBUG_UART_PORT, settings);

	port_setup = true;

	return port.setup();
}

static void print_msg(const char* string, size_t len){
	if (port_setup){
		port.transmit((uint8_t*)string, len);
	}
}

void debug(const char* string){
	int len = sprintf(buffer, "[DEBUG] %s\r\n", string);
	print_msg(buffer, (size_t)len);
}

void info(const char* string){
	int len = sprintf(buffer, "[INFO] %s\r\n", string);
	print_msg(buffer, (size_t)len);
}

void abort(const char* msg,const char* file, int line){
	int len = sprintf(buffer, "[ABORTED] %s FILE: %s LINE %d", msg, file, line);
	print_msg(buffer, (size_t)len);
	while(1);
}