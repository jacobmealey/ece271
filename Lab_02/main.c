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
#define gpioc_mode 0x0000055;
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
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

unsigned char message[64] = "moo";
unsigned char ColumnPressed, RowPressed;


void I2C_GPIO_init(void);
void keypad_scan(unsigned char * key);
int main(void){
	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	I2C_GPIO_init();
	I2C_Initialization(I2C1);

	ssd1306_Init();
	ssd1306_Fill(Black);
	while(1){
		keypad_scan(&key);
		
		message[0] =  key;
		message[1] = '\0';
		ssd1306_SetCursor(2,0);
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_UpdateScreen();		
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

void keypad_scan(unsigned char *key){
	int i, j, d;
	// Set all output pins low
	GPIOC->ODR |= (row_masks[0] | row_masks[1] | row_masks[2] |  row_masks[3]);
	for(i = 1; i < 4; i++){
		if((GPIOC->IDR & col_masks[i]) != (col_masks[i])){
			ColumnPressed = i;
		}
	}
	GPIOC->ODR &= ~(row_masks[0] | row_masks[1] | row_masks[2] |  row_masks[3]);
	for(d=0;d<25;d++);
	for(j =0; j < 4; j++){
		GPIOC->ODR &= ~(row_masks[j]);
		if((GPIOC->IDR & col_masks[ColumnPressed]) != (col_masks[ColumnPressed])){
			RowPressed = j;
			*key = keypad[RowPressed][ColumnPressed];
		}
		GPIOC->ODR |= row_masks[j];
	}
	
}


