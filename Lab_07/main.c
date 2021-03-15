#include "stm32l476xx.h"
// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

double cycle;	
double cycle_increment;
int state = 0;
// User HSI (high-speed internal) as the processor clock


void configure_LED_pin(){
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

void turn_on_LED(){
	GPIOA->ODR |= 1 << LED_PIN;
}

void turn_off_LED(){
	GPIOA->ODR &= ~(1 << LED_PIN);
}

void toggle_LED(){
	GPIOA->ODR ^= (1 << LED_PIN);
}



int main(void){
	
	while(1);
	
}
