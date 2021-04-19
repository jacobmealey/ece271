#include "stm32l476xx.h"
#include "I2C.h"
#include "I2C_gpio.h"
#include "ssd1306.h"
// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

double cycle;	
double cycle_increment;
volatile uint32_t pulse_width = 0;
volatile uint32_t last_captured = 0;
volatile uint32_t signal_polarity = 0;
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
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	// Set up PB.6 as AF 
	GPIOB->MODER &= ~(3UL << 2 * 6);
	GPIOB->MODER |=   2UL << 2 * 6;
	
	// Set up PB.5 as AF
	GPIOB->MODER &= ~(3UL << 2 * 5);
	GPIOB->MODER |=  	2UL << 2 * 5;
}

void setup_clock(void){
	// Enable timer 4
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	// Set prescalar and ARR
	TIM4->PSC = 16; // prescale to 1MHz
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
	
	// Enable Timer 1
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
	TIM3->PSC = 16;
	// Period to 100us
	TIM3->ARR = 0xFFFF;
  //This makes the output high for 10us every 65ms
  TIM3->CCMR1 &= ~TIM_CCMR1_OC2M;
  TIM3->CCMR1 |= (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);

  //Select active high polarity 
  TIM3->CCER &= ~TIM_CCER_CC2P;
  TIM3->CCER |= TIM_CCER_CC2P;

	TIM3->BDTR |= TIM_BDTR_MOE;
	
	// Set DC to 10% (pulsewidth 10us)
	TIM3->CCR1 = 10;
	
	TIM3->CR1 |= TIM_CR1_CEN;
	
	
}

void TIM4_IRQHandler(void){
	uint32_t current_captured;
	// check interrupt flag
	if(!(TIM4->SR & TIM_SR_CC1IF)) {
		current_captured = TIM4->CCR1;
		signal_polarity = 1 - signal_polarity;
		
		if(!signal_polarity)
			pulse_width += current_captured - last_captured;
		
		last_captured = current_captured;
	}
	
	if((TIM4->SR & TIM_SR_UIF) != 0)
		TIM4->SR &= ~TIM_SR_UIF;
}

void int_to_string(uint32_t num, char *string){
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0){
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++){
        rem = num % 10;
        num = num / 10;
        string[len - (i + 1)] = rem + '0';
    }
    string[len] = '\0';

}
int main(void){
	char str[10];
	enable_HSI();
	//I2C_GPIO_init();
	//ssd1306_Init();
	setup_gpio();
	setup_clock();
	
  cycle	= 0.0;
  cycle_increment	= 0.01;

	while(1){
//		int_to_string(pulse_width, str);
//		ssd1306_Fill(Black);
//		ssd1306_SetCursor(0, 2);
//		ssd1306_WriteString("poo poo pee pee", Font_6x8, White);
//		ssd1306_SetCursor(0,10);
//		ssd1306_WriteString(str, Font_6x8, White);
//		ssd1306_UpdateScreen();
	}
}
