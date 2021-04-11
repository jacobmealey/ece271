#include "stm32l476xx.h"
// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

double cycle;	
double cycle_increment;
int state = 0;
int TimeDelay = 0;
volatile int wiper_speed;
volatile int is_wiping;
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


void enable_gpio(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	GPIOC->MODER &= ~(3UL << (2 * LED_PIN));
	GPIOC->MODER |=  	0UL << (2 * LED_PIN);
}
void EXTI15_10_IRQHandler(void){
	// Check to make sure its the right pin
	if((EXTI->PR1 & EXTI_PR1_PIF13) == EXTI_PR1_PIF13){
		if(wiper_speed <= 750){
			is_wiping = 1;
			wiper_speed += 250;
		}else{
			wiper_speed = 0;
			is_wiping = 0;
		}

		EXTI->PR1 |= EXTI_PR1_PIF13;
	}
}
void EXTI_Init(void){
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

void configure_devices(){

  // Enable the clock to GPIO Port A	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	GPIOA->AFR[0] &= ~(0xF); 
	GPIOA->AFR[0] |= 1UL << 20; //set PA5 to AF1 for TIM2 CH1
	GPIOA->AFR[0] &= ~(0xF); 
	GPIOA->AFR[0] |= 2UL; //set PA0 to AF2 for TIM5 CH1

		

	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOA->MODER &= ~(3UL<<(2*LED_PIN));  
	GPIOA->MODER &= ~(3UL);
	GPIOA->MODER |= 2UL<<(2*LED_PIN);      // AlternateFunction(10)
	GPIOA->MODER |= 2UL;
	GPIOC->MODER &= ~(3UL<<(2*BUTTON_PIN));

	

	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOA->OSPEEDR &= ~(3<<(2*LED_PIN));
	GPIOA->OSPEEDR |=   2<<(2*LED_PIN);  // Fast speed	
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOA->OTYPER &= ~(1<<LED_PIN);      // Push-pull
	GPIOC->OTYPER &= ~(1<<BUTTON_PIN);      // Push-pull

	

	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)

	GPIOA->PUPDR  &= ~(3<<(2*LED_PIN));  // No pull-up, no pull-down

}



void Timer2_Initialization(){
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	
	TIM2->CR1 &= ~(1UL); //set direction to up-counting
	TIM2->PSC = 39; // Clock speed = 100KHz = (4MHz/(1+39))
	TIM2->ARR = 999; // Period = 0.01s = (999+1)*(1/100KHz)

	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M; 
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; //set to PWM mode 1
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE; //enable preload
	TIM2->CCER &= ~TIM_CCER_CC1NP; //set output polarity
	TIM2->BDTR |= TIM_BDTR_MOE; //enable main output
	TIM2->CCER |= ~TIM_CCER_CC1NE; //enable complementary output

	TIM2->CCR1 = 500;
	TIM2->CR1 |= TIM_CR1_CEN; //enable counter

}



void Timer5_Initialization(){

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;

	

	TIM5->CR1 &= ~(1UL); //set direction to up-counting
	TIM5->PSC = 39; // Clock speed = 100KHz = (4MHz/(1+39))
	TIM5->ARR = 1999; // Period = 0.02s = (1999+1)*(1/100KHz)

	TIM5->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM5->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; //set to PWM mode 1
	TIM5->CCMR1 |= TIM_CCMR1_OC1PE; //enable preload
	TIM5->CCER &= ~TIM_CCER_CC1NP; //set output polarity
	TIM5->BDTR |= TIM_BDTR_MOE; //enable main output
	TIM5->CCER |= ~TIM_CCER_CC1NE; //enable complementary output
	 
	TIM5->CCR1 = 1900;
	TIM5->CR1 |= TIM_CR1_CEN; //enable counter

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

void SysTick_Initialize(uint32_t ticks){
	SysTick->CTRL = 0;
	SysTick->LOAD = ticks - 1;
	
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	
	SysTick->VAL = 0;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void Delay(uint32_t nTime){
	TimeDelay = nTime;
	while(TimeDelay != 0);
}

void SysTick_Handler(void){
	if(TimeDelay > 0)
		TimeDelay--;
}


int main(void){
	int output = 500;
	int dir = 1;
	//enable_HSI();
	configure_devices();
	Timer2_Initialization();
	Timer5_Initialization();
	SysTick_Initialize(8000);
	enable_gpio();
	EXTI_Init();
  // Dead loop & program hangs here

  //cycle	= 0.0;
  //cycle_increment	= 0.01;

	TIM5->CCR1 = 1750; //Rotate motor by specified angle
	Delay(1000);
	TIM5->CCR1 = 1870;
	Delay(1000);

	while(1){
		if(is_wiping){
			TIM5->CCR1 = 1750;
			Delay(1000 - wiper_speed);
			TIM5->CCR1 = 1870;
			Delay(1000 - wiper_speed);
		}
//		if(output > 999 || output < 0){
//			dir = -dir;
//		}
//		output += dir;
//		TIM2->CCR1 = output;
//		Delay(1);
	}
		
}
