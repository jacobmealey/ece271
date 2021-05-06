#include "stm32l476xx.h"

// return 1 if leap year; otherwise 0
extern uint32_t leap_year(uint32_t year);

int main(void){
	
	volatile uint32_t count = 0;
	uint32_t year;
	
	// There are 30 leap years between 1900 and 2020.
	for (year=1900; year<2021; year++) {
		count += leap_year(year);
	}
	while(1);
}

