/* ECE 271 - Lab 07
 * Author: Jacob Mealey <jacob.mealey@maine.edu>
 * Stop Watch
 */
#include "stm32l476xx.h"
#include "I2C.h"
#include "I2C_gpio.h"
#include "ssd1306.h"
#include "SysClock.h"
// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

double cycle;	
double cycle_increment;
int state = 0;
// User HSI (high-speed internal) as the processor clock
int TimeDelay = 0;
//int total_time; // Time ellapsed in ms
static uint8_t  active;
int time_mseconds = 0;

void configure_GPIO();
void EXTI_Init(void);
void SysTick_Initialize(uint32_t ticks);
void Delay(uint32_t nTime);
void time(int t_time, int *mins, int *secs);
void int_to_string(int num, char *string);

int main(void){
	// I want the delay to work in milliseconds
	// so 4MHz -> 4000 ticks persec
	int minutes = 0, seconds = 0;
	char time_string[5];
	active = 0;
	
	
	System_Clock_Init(); 				// this function sets the clock to 80Mhz from the i2c
	SysTick_Initialize(80000);	// set the interupts to trigger every 80000 Ticks (once every ms)
	I2C_GPIO_init();					
	ssd1306_Init();
	EXTI_Init();								// Enable EXTI to stop and start the timer.
	configure_GPIO();
	
	ssd1306_Fill(Black);
	time_mseconds = 0;

	while(1){
		// Something Cool
		// this section is kind of intense.  but basically
		// It just gets the times from variables and writes it
		// as mm:ss.xxxx (minutes, seconds, miliseconds)
		time(time_mseconds, &minutes, &seconds);
		ssd1306_SetCursor(0, 2);
		// Write Minutes
		if(minutes < 10){
			ssd1306_WriteChar('0', Font_6x8, White);
		}
		int_to_string(minutes, time_string);
		ssd1306_WriteString(time_string, Font_6x8, White);
		
		ssd1306_WriteChar(':', Font_6x8, White);
		// Write Seconds
		if(seconds < 10){
			ssd1306_WriteChar('0', Font_6x8, White);
		}
		int_to_string(seconds, time_string);
		ssd1306_WriteString(time_string, Font_6x8, White);
		ssd1306_WriteChar('.', Font_6x8, White);
		
		//Milliseconds
	  int_to_string(time_mseconds % 1000, time_string);
		ssd1306_WriteString(time_string, Font_6x8, White);
		ssd1306_UpdateScreen();

// 	// Lab Demo
//		toggle_LED();
//		Delay(1000);
	}
	
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
void EXTI15_10_IRQHandler(void){
	if((EXTI->PR1 & EXTI_PR1_PIF13) == EXTI_PR1_PIF13){
		active = !active;
		EXTI->PR1 |= EXTI_PR1_PIF13;
	}
}

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
	if(!active){
		time_mseconds += 1;
	}
	if(TimeDelay > 0)
		TimeDelay--;
}



void time(int t_time, int *mins, int *secs){
	int temp_time =  t_time / 1000; // Convert to seconds
	*mins = temp_time / 60;
	*secs = temp_time % 60;
}


void int_to_string(int num, char *string){
		int i, rem, len = 0, n;
		if(num == 0){
			string[0] = '0';
			string[1] = '\0';
			return;
		}
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


