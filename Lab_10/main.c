#include "stm32l476xx.h"
// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

double cycle;	
double cycle_increment;
int state = 0;
// User HSI (high-speed internal) as the processor clock
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

void setup_gpio(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN | RCC_AHB2ENR_GPIOBEN;
	// Set up PB.6 as AF 
	GPIOB->MODER &= ~(3UL << 2 * 6);
	GPIOB->MODER |=   2UL << 2 * 6;
	
	// Set up PE.11 as AF
	GPIOE->MODER &= ~(3UL << 2 * 11);
	GPIOE->MODER |=  	2UL << 2 * 11;
}

void setup_clock(void){
	// Enable timer 4
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	// Set prescalar and ARR
	TIM4->PSC = 15999;
	TIM4->ARR = 0xFFFF;
	
	// ??? what ???
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_1;
	TIM4->CCMR1 &= ~(TIM_CCMR1_IC1F);
	TIM4->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1NP;
	
	// Clear input prescaler 
	TIM4->CCMR1 &= ~TIM_CCMR1_IC1PSC;
	
	// enable interrupt generation 
	TIM4->DIER |= TIM_DIER_UIE;
	
	// enable time 4 :)
	TIM4->CR1 |= TIM_CR1_CEN;
	NVIC_SetPriority(TIM4_IRQn, 0);
	
	NVIC_EnableIRQ(TIM4_IRQn);
	
}

int main(void){

	enable_HSI();
	//turn_on_LED();
  // Dead loop & program hangs here

  cycle	= 0.0;
  cycle_increment	= 0.01;

	while(1){
		
	}
}
