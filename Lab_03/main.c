/* ECE 331 Lab 03 
 * Author: Jacob Mealey <jacob.mealey@maine.edu>
 * Date Last Modified: 02/09/21:w
 */

#include "stm32l476xx.h"
#include "SysClock.h"
#include "I2C.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"

// Note: The code gets Warning L6989W
// Report: this is an known issue for STM32L4xx.
// https://www.keil.com/support/docs/3853.htm
// We can ignore this warning because we are not using
// external memory.

// PB 8 <--> AF4 (I2C1_SCL)
// PB 9 <--> AF4 (I2C1_SDA)
#define I2C_SCL_PIN  8
#define I2C_SDA_PIN  9
#define I2C_PORT     GPIOB
#define gpioc_mode 0x0055455;
unsigned char key;
uint8_t Data_Receive[6];
uint8_t Data_Send[6];

unsigned int row_masks[] = {
		GPIO_ODR_ODR_0, 
		GPIO_ODR_ODR_1, 
		GPIO_ODR_ODR_2, 
		GPIO_ODR_ODR_3
	};
	
	unsigned int col_masks[] = {
		GPIO_IDR_IDR_4,
		GPIO_IDR_IDR_10,
		GPIO_IDR_IDR_11,
		GPIO_IDR_IDR_12
	};

unsigned char keypad[4][4] = 
{
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'*', '0', '#', 'D'},
	{'7', '8', '9', 'C'}
	
};


char message[64] = "";
unsigned char ColumnPressed, RowPressed;
int pos;

int motor_pos;

void I2C_GPIO_init(void);
unsigned char keypad_scan(void);
void stinky_delay(int ticks);
void full_step(int dir, int step);
void half_step(int dir, int step);
int string_to_int(char * string, int len);
int clear_str(char * string, int * len);

int main(void){
	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	I2C_GPIO_init();
	I2C_Initialization(I2C1);

	ssd1306_Init();
	ssd1306_Fill(Black);
	motor_pos = 0;
	while(1){
		
		half_step(1, 100);
		stinky_delay(100);
	}	 // Deadloop
}

// Configure I2C1_SCL (PB8) Pin as : Alternate function, High Speed, Open drain, Pull up 
// Configure I2C1_SDA (PB9) Pin as : Alternate function, High Speed, Open drain, Pull up
// PB 8 <--> AF4 (I2C1_SCL)
// PB 9 <--> AF4 (I2C1_SDA)

void I2C_GPIO_init(void){

	RCC->AHB2ENR  |=  RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN;
	GPIOC->MODER &= 0x00000000;
	GPIOC->MODER |= gpioc_mode;
	// set outputs to opendrain, turn them into variables?
	GPIOC->OTYPER |= 1U<< 0 | 1U<<(2*1) | 1U<<(2*2) | 1U<<(2*3);
	GPIOC->PUPDR	|= 1U<<(2*4) 	| 1U<<(2*10) | 1U<<(2*11) | 1U<<(2*12);
	
	// Set STEPPER pins to push pull
	GPIOC->OTYPER &= ~(1<<(2*5) | 1<<(2*6) | 1<<(2*8) | 1<<(2*9));
	
	// GPIO Mode: Input(00, reset), Output(01), AlterFunc(10), Analog(11, reset)
	I2C_PORT->MODER   &= ~( 3U<<(2*I2C_SCL_PIN) | 3U<<(2*I2C_SDA_PIN) );  // Clear Mode	
	I2C_PORT->MODER   |=    2U<<(2*I2C_SCL_PIN) | 2U<<(2*I2C_SDA_PIN);    // Alternative Function
	
	I2C_PORT->AFR[1]  &= ~0x000000FF;   // Alternative Function 4 = I2C1
	I2C_PORT->AFR[1]  |=  0x00000044;   // Alternative Function 4 = I2C1
	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	I2C_PORT->OSPEEDR |=    3U<<(2*I2C_SCL_PIN) | 3U<<(2*I2C_SDA_PIN);    // High Speed
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	I2C_PORT->PUPDR   &= ~( 3U<<(2*I2C_SCL_PIN) | 3U<<(2*I2C_SDA_PIN) );
	I2C_PORT->PUPDR   |=    1U<<(2*I2C_SCL_PIN) | 1U<<(2*I2C_SDA_PIN) ;   // Pull-up
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
	I2C_PORT->OTYPER  |= 1U<<I2C_SCL_PIN | 1U<<7;  // Open Drain 
}

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

unsigned char keypad_scan(void){
	int i, j, d;
	unsigned char new_key;
	new_key = 0xFF;
	GPIOC->ODR |= (row_masks[0] | row_masks[1] | row_masks[2] |  row_masks[3]);
	for(j = 0; j < 4; j++){
		GPIOC->ODR &= ~(row_masks[j]);
		for(d=0;d<25;d++);
		for(i = 0; i < 4; i++){
			if((GPIOC->IDR & col_masks[i]) == 0x00){
				new_key = keypad[j][i];
				while((GPIOC->IDR & col_masks[i]) == 0x00);
			}
		}

		GPIOC->ODR ^= row_masks[j];
	}
	return new_key;
}


void stinky_delay(int ticks){
	int d;
	for(d = 0; d < ticks; d++);
}

int string_to_int(char * string, int len){
	int i, num = 0;
	for(i = 0; i < len; i++){
		num = (num * 10) + (string[i] - '0');
	}
	return num;
}

int clear_str(char * string, int * len){
	int pos;
	for(pos = 0; pos < *len; pos++){
		message[pos] = ' ';
	}
	*len = 0;
}
