#include "stepper.h"

void full_step(int dir, int step){
	int i, current_step;
	
	short bits[4] = {
		0x9, //0b1001
		0xA, //0b1010
		0x6, //0b0110
		0x5  //0b0101
	};
	if(dir){
		for(current_step = 0; current_step < step; current_step++){
			// This is the loop for a single step clockwise
			for(i = 0; i < 4; i++){
				GPIOC->ODR &= ~(GPIO_IDR_IDR_5);
				GPIOC->ODR &= ~(GPIO_IDR_IDR_8);
				GPIOC->ODR &= ~(GPIO_IDR_IDR_6);
				GPIOC->ODR &= ~(GPIO_IDR_IDR_9);
				GPIOC->ODR |= ((bits[i]>>3U) & 0x1) << 5U; // set pin 5 (A) to MSB of bits[i]
				GPIOC->ODR |= ((bits[i]>>2U) & 0x1) << 6U; // set pin 6 (~A) to 3rd bit of bits[i]
				GPIOC->ODR |= ((bits[i]>>1U) & 0x1) << 8U; // set pin 8 (B) to 2nd bit of bits[i]
				GPIOC->ODR |= ((bits[i]) & 0x1) << 9U; // set pin 9 (~B) to LSB of bits[i]
				stinky_delay(60000);
			}
		}
	}else if(!dir){
		for(current_step = 0; current_step < step; current_step++){
		// This is the loop for a single step counter clockwise
			for(i = 3; i >= 0; i--){
				GPIOC->ODR &= ~(GPIO_IDR_IDR_5);
				GPIOC->ODR &= ~(GPIO_IDR_IDR_8);
				GPIOC->ODR &= ~(GPIO_IDR_IDR_6);
				GPIOC->ODR &= ~(GPIO_IDR_IDR_9);
				GPIOC->ODR |= ((bits[i]>>3U) & 0x1) << 5U; // set pin 5 (A) to MSB of bits[i]
				GPIOC->ODR |= ((bits[i]>>2U) & 0x1) << 6U; // set pin 6 (~A) to 3rd bit of bits[i]
				GPIOC->ODR |= ((bits[i]>>1U) & 0x1) << 8U; // set pin 8 (B) to 2nd bit of bits[i]
				GPIOC->ODR |= ((bits[i]) & 0x1) << 9U; // set pin 9 (~B) to LSB of bits[i]
				stinky_delay(60000);
			}
		}
	}
}


void half_step(int dir, int step){
	// A half step is 8 sequences long
	// We're gonna want an array where each bit 
	// Corresponds to an outout
	// MSB is A and LSB is ~B
	// 0b1001 
	short bits[8] = {
		0x9, // 0b1001
		0x8, // 0b1000
		0xA, // 0b1010
		0x2, // 0b0010
		0x6, // 0b0110
		0x4, // 0b0100
		0x5, // 0b0101
		0x1, // 0b0001
	};
	int i, current_step;
	if(dir){
		for(current_step = 0; current_step < step; current_step++){
			// This is the loop for a single step clockwise
			for(i = 0; i < 8; i++){
				GPIOC->ODR &= ~(GPIO_IDR_IDR_5);
				GPIOC->ODR &= ~(GPIO_IDR_IDR_8);
				GPIOC->ODR &= ~(GPIO_IDR_IDR_6);
				GPIOC->ODR &= ~(GPIO_IDR_IDR_9);
				GPIOC->ODR |= ((bits[i]>>3) & 0x1) << 5; // set pin 5 (A) to MSB of bits[i]
				GPIOC->ODR |= ((bits[i]>>2) & 0x1) << 6; // set pin 6 (~A) to 3rd bit of bits[i]
				GPIOC->ODR |= ((bits[i]>>1) & 0x1) << 8; // set pin 8 (B) to 2nd bit of bits[i]
				GPIOC->ODR |= ((bits[i]) & 0x1) << 9; // set pin 9 (~B) to LSB of bits[i]
				stinky_delay(30000);
			}
		}
	}else if(!dir){
		for(current_step = 0; current_step < step; current_step++){
		// This is the loop for a single step counter clockwise
			for(i = 7; i >= 0; i--){
				GPIOC->ODR &= ~(GPIO_IDR_IDR_5);
				GPIOC->ODR &= ~(GPIO_IDR_IDR_8);
				GPIOC->ODR &= ~(GPIO_IDR_IDR_6);
				GPIOC->ODR &= ~(GPIO_IDR_IDR_9);
				GPIOC->ODR |= ((bits[i]>>3) & 0x1) << 5; // set pin 5 (A) to MSB of bits[i]
				GPIOC->ODR |= ((bits[i]>>2) & 0x1) << 6; // set pin 6 (~A) to 3rd bit of bits[i]
				GPIOC->ODR |= ((bits[i]>>1) & 0x1) << 8; // set pin 8 (B) to 2nd bit of bits[i]
				GPIOC->ODR |= ((bits[i]) & 0x1) << 9; // set pin 9 (~B) to LSB of bits[i]
				stinky_delay(30000);
			}
		}
	}
}
