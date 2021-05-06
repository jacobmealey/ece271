#include "stm32l476xx.h"

extern float min(float a, float b, float c, float d);

// Note: since we are using FPU, you should
// connect the STM32L4 kit to the computer..

int main(void){
	
	volatile float e;

	e = min(1.11, 2.22, -3.33, -4.44); // return -4.44
  e = min(1.11, -4.44, -3.33, 2.22); // return -4.44
  e = min(-3.33, 2.22, -4.44, 1.11); // return -4.44
  e = min(-4.44, -3.33, 1.11, 2.22); // return -4.44
  e = min(2.22, -3.33, 1.11, -4.44); // return -4.44
	e = min(2.22, -4.44, 1.11, -3.33); // return -4.44
	e = min(-3.33, -4.44, 1.11, 2.22); // return -4.44

	while(1);
}

