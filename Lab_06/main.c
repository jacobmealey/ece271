/* ECE 271 - Lab 06
 * Author: Jacob Mealey <jacob.mealey@maine.edu>
 * Button counter.
 */
#include "stm32l476xx.h"
#include "stm32l476xx.h"
#include "SysClock.h"
#include "I2C.h"
#include "I2C_gpio.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include <string.h>
// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

int total_presses = 0;
char str[100];

double cycle;	
double cycle_increment;
int state = 0;
void int_to_string(int num, char *string);
void EXTI_Init(void);
void configure_GPIO(void);
void turn_on_LED(void);
void turn_off_LED(void);
void toggle_LED(void);

int main(void){

	System_Clock_Init();
	I2C_GPIO_init();
	ssd1306_Init();
	ssd1306_Fill(Black);
	configure_GPIO();
	//turn_on_LED();
  // Dead loop & program hangs here
	EXTI_Init();
	turn_on_LED();
	while(1){
		ssd1306_SetCursor(0, 2);
		ssd1306_WriteString("Pressed ", Font_6x8, White);
		if(total_presses){
			ssd1306_WriteString(str, Font_6x8, White);
		}else{
			ssd1306_WriteString("0", Font_6x8, White);
		}
		ssd1306_WriteString(" Times.", Font_6x8, White);
		ssd1306_UpdateScreen();
		int_to_string(total_presses, str);
	}
}

void int_to_string(int num, char *string){
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0){
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++){
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';

}

void enable_HSI(){
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 ) {;}
	
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;  //01: HSI16 oscillator used as system clock

  // Wait till HSI is used as system clock source 
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 ) {;}
}

void configure_GPIO(){
  // Enable the clock to GPIO Port A	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN;   
	
	GPIOC->MODER &= ~(3UL<<(2*BUTTON_PIN));
	GPIOC->MODER |= 0UL<<(2*BUTTON_PIN);
	
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOA->MODER &= ~(3UL<<(2*LED_PIN));  
	GPIOA->MODER |=   1UL<<(2*LED_PIN);      // Output(01)
	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	//////GPIOA->OSPEEDR &= ~(3<<(2*LED_PIN));
	//GPIOA->OSPEEDR |=   2<<(2*LED_PIN);  // Fast speed
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOA->OTYPER &= ~(1<<LED_PIN);      // Push-pull
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOA->PUPDR  &= ~(3<<(2*LED_PIN));  // No pull-up, no pull-down
}

void turn_on_LED(void){
	GPIOA->ODR |= 1 << LED_PIN;
}

void turn_off_LED(void){
	GPIOA->ODR &= ~(1 << LED_PIN);
}

void toggle_LED(void){
	GPIOA->ODR ^= (1 << LED_PIN);
}



void EXTI15_10_IRQHandler(void){
	// Check to make sure its the right pin
	if((EXTI->PR1 & EXTI_PR1_PIF13) == EXTI_PR1_PIF13){
		toggle_LED();
		total_presses++;
		EXTI->PR1 |= EXTI_PR1_PIF13;
	}
}

// Enables the EXIT for pin pc.13
void EXTI_Init(void) {
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	//NVIC->ISER[1] = 1 << 8;
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;
	EXTI->RTSR1 |=  EXTI_RTSR1_RT13;
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT13;
	EXTI->IMR1 	|=  EXTI_IMR1_IM13;
	NVIC_SetPriority(EXTI15_10_IRQn, 0);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	
}
