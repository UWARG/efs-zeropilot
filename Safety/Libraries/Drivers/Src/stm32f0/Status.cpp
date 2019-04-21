#include <stdio.h>
#include "Status.hpp"

void abort(const char* msg,const char* file, int line){
	printf("%s FILE: %s LINE %d", msg, file, line);
	while(1);
}