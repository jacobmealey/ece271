#include "stm32l476xx.h"

extern int8_t compare64(int64_t a, int64_t b);

int main(void){
	
	volatile int8_t result = 0;

	result = compare64(1, 2);  // return -1
	result = compare64(1, 1);  // return 0
	result = compare64(2, 1);  // return 1
	
	result = compare64(-1, -2);  // return 1
	result = compare64(-1, -1);  // return 0
	result = compare64(-2, -1);  // return -1
	
	result = compare64(-1, 0);  // return -1
	result = compare64(0, -1);  // return 1
	while(1);
}

