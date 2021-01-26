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

// This isn't the best way to do a delay, but time is of
// the essence, so here we are.  
void stinky_delay(int ticks){
	int i;
	for(i = 0; i < ticks; i++);
}

// This is a function that outputs on pwm pulse, it needs to be
// run in a loop in order to get a full pwm effect.
void foux_pwm(double duty_cycle){
	int max_time = 40000;
	turn_on_LED();
	stinky_delay((int) max_time*duty_cycle);
	turn_off_LED();
	stinky_delay(max_time - max_time*duty_cycle);
}

int main(void){

	enable_HSI();
	configure_LED_pin();
	//turn_on_LED();
  // Dead loop & program hangs here

  cycle	= 0.0;
  cycle_increment	= 0.01;

	while(1){
		/*
		if((GPIOC->IDR & GPIO_IDR_IDR_13) == GPIO_IDR_IDR_13){
			//stinky_delay(1000);
			toggle_LED();
			while((GPIOC->IDR & GPIO_IDR_IDR_13) != 0x00);
		}
		*/
		
		foux_pwm(cycle);
		if(cycle >= 1.0){
			cycle_increment = -0.01;
		} else if(cycle <= 0.0){
			cycle_increment = 0.01;
		}	
		cycle += cycle_increment;

		
	}
}
