#include "stm32l476xx.h"
// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13
#define DAC_IN 01
double cycle;	
double cycle_increment;
int state = 0;
int adc_in;

void ADC_Wakeup (void);
void enable_ADC (void);
void enable_HSI (void);
void gpio_init (void);

int main(void){
	enable_HSI();
	enable_ADC();
	gpio_init();
	ADC_Wakeup();
	while(1){
		ADC1->CR |= ADC_CR_ADSTART;
		while((ADC123_COMMON->CSR & ADC_CSR_EOC_MST) != ADC_CSR_EOC_MST);
		adc_in = ADC1->DR;
	}
}

void enable_ADC (void) {
	// enable ADC clock
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	// Turn off ADC 
	ADC1->CR &= ~(ADC_CR_ADEN);
	// enable analog voltage booster
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;
	// Configure CCR
	ADC123_COMMON->CCR |= ADC_CCR_VREFEN;
	ADC123_COMMON->CCR &= ADC_CCR_PRESC;
	ADC123_COMMON->CCR |= ADC_CCR_CKMODE_0;
	ADC123_COMMON->CCR &= ~(ADC_CCR_DUAL);
	// set right align to 12 bit
	ADC1->CFGR |= ADC_CFGR_ALIGN;
	// set conversion setting six
	ADC1->SQR1 &= ~ADC_SQR1_L;
	ADC1->SQR1 &= ~ADC_SQR1_SQ1;
	ADC1->SQR1 |= (6U << 6);
	// Select channel 6 
	ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_6;
	ADC1->SMPR1 |= ADC_SMPR1_SMP6_0;
	ADC1->CFGR &= ~ADC_CFGR_CONT;
	ADC1->CFGR &= ~ADC_CFGR_EXTEN;
	ADC1->CR |= ADC_CR_ADEN;
	// Wait for adc to be ready
	while(!(ADC1->ISR | ADC_ISR_ADRDY));
}


void gpio_init(void){
	// Enable LED_PIN 
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~(3UL << (2 * LED_PIN));
	GPIOA->MODER |=  	1UL << (2 * LED_PIN);
	
	// Set up pin A1 as as analog
	GPIOA->MODER |= 3UL << (2 * DAC_IN);
	GPIOA->PUPDR &= ~(3UL << (2 * DAC_IN));
	GPIOA->ASCR  |= 1U << DAC_IN;
}



void enable_HSI(void){
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

void ADC_Wakeup (void) {
 int wait_time;
 // Exit deep power down mode if still in that state
 // DEEPPWD = 0: ADC not in deep-power down
 // DEEPPWD = 1: ADC in deep-power-down (default reset state)
 if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD)
 ADC1->CR &= ~ADC_CR_DEEPPWD;
 // Enable the ADC internal voltage regulator
 // Before performing any operation such as launching a calibration or enabling the ADC,
 // the ADC voltage regulator must first be enabled and the software must wait for the
 // regulator start-up time.
 ADC1->CR |= ADC_CR_ADVREGEN;
 // Wait for ADC voltage regulator start-up time
 // The software must wait for the startup time of the ADC voltage regulator
 // (T_ADCVREG_STUP, i.e. 20 us) before launching a calibration or enabling the ADC.
wait_time = 20 * (80000000 / 1000000);
 while (wait_time != 0) {
 wait_time--;
 }
}

