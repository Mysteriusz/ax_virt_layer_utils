#include "math.h"

unsigned int log_b(
	unsigned int arg,
	unsigned int base
){
	unsigned int count = 0;
	while(arg >= base){
		count++;
		arg /= base;
	}

	return count;
}
