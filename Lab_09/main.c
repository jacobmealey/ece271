#include "stm32l476xx.h"
// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13
#define DAC_IN 01
double cycle;	
double cycle_increment;
int state = 0;
uint32_t volatile ADC_Results;
uint8_t volatile DMA_DONE = 0;
//volatile uint16_t adc_in;

void ADC_Wakeup (void);
void enable_ADC (void);
void enable_HSI (void);
void gpio_init (void);
void enable_dma(void);
void config_timer(void);
int main(void){
	//uint32_t adc_in = 0;
	config_timer();
	enable_HSI();
	enable_dma();
	enable_ADC();
	ADC_Wakeup();
	gpio_init();
	while(1){
		//ADC1->CR |= ADC_CR_ADSTART;
		//while(!DMA_DONE);
		
		if((ADC_Results) < 0x7000){
			GPIOA->ODR &= ~(1 << LED_PIN);
		}else{
			GPIOA->ODR |= 1 << LED_PIN;
		}
		//DMA_DONE = 0;
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
	
	ADC1->CFGR &= ~ADC_CFGR_EXTSEL;
	ADC1->CFGR |= ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_2;
	ADC1->CFGR &= ~ADC_CFGR_EXTEN;
	ADC1->CFGR |= ADC_CFGR_EXTEN_0;
	ADC1->CFGR |= ADC_CFGR_DMACFG;
	ADC1->CFGR |= ADC_CFGR_DMAEN;
	ADC1->CR |= ADC_CR_ADSTART;
	ADC1->CR |= ADC_CR_ADEN;
	// Wait for adc to be ready
	while(!(ADC1->ISR | ADC_ISR_ADRDY));
}

void enable_dma(){
	// enable DMA 1 clock
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	// Disable mem2mem
	DMA1_Channel1->CCR &= ~DMA_CCR_MEM2MEM;
	
	// Set channel 1 to high priority level
	DMA1_Channel1->CCR &= DMA_CCR_PL;
	DMA1_Channel1->CCR |= DMA_CCR_PL_1;
	
	DMA1_Channel1->CCR &= ~DMA_CCR_PSIZE;
	DMA1_Channel1->CCR |= DMA_CCR_PSIZE_1;
	
	// Set Peripheral size to 16 bits
	DMA1_Channel1->CCR &= ~DMA_CCR_MSIZE;
	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;
	
	// Disable peripheral increment mode
	DMA1_Channel1->CCR &= ~DMA_CCR_PINC;
	
	// Enable memory increment mode
	DMA1_Channel1->CCR |= DMA_CCR_MINC;
	
	// Enable Circular Mode
	DMA1_Channel1->CCR |= DMA_CCR_CIRC;
	
	// Set data transfer from peripheral to memory
	DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
	
	// Amount of data to trasnfer 
	DMA1_Channel1->CNDTR = 1;
	
	// Have DMA peripheral address register point to
	// ADC data register (assumes ADC is in initialized;
	DMA1_Channel1->CPAR = (uint32_t) &(ADC1->DR);
	
	// Have DMA store in ADC_Results
		DMA1_Channel1->CMAR = (uint32_t)&ADC_Results;
	
	// DMA Channale select
	// Map DMA 1 to ADC1
	DMA1_CSELR->CSELR &= ~DMA_CSELR_C1S;
	
	// Enable DMA Channel 1 interrupt
	DMA1_Channel1->CCR |= DMA_CCR_TCIE;
	
	//NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	DMA1_Channel1->CCR |= DMA_CCR_EN;

}

void DMA1_Channel1_IRQHandler(void) {
	if((DMA1->ISR & DMA_ISR_TCIF1) == DMA_ISR_TCIF1){
		DMA1->IFCR |= DMA_IFCR_CTCIF1;
		
		DMA1_Channel1->CMAR = (uint32_t) &ADC_Results;
		DMA_DONE = 1;
	}
	DMA1->IFCR |= (DMA_IFCR_CHTIF1 | DMA_IFCR_CGIF1 | DMA_IFCR_CTEIF1);
}

void gpio_init(void){
	// Enable LED_PIN 
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOA->MODER &= ~(3UL<<(2*LED_PIN));  
	GPIOA->MODER |=   1UL<<(2*LED_PIN);      // Output(01)
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOA->OTYPER &= ~(1<<LED_PIN);      // Push-pull
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOA->PUPDR  &= ~(3<<(2*LED_PIN));  // No pull-up, no pull-down
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

void config_timer(void) {
	// 1 MHz clock

	// Enablbe clock
	RCC->APB1ENR1 |=  RCC_APB1ENR1_TIM4EN;
	
	TIM4->CR1 &= ~TIM_CR1_CMS;
	TIM4->CR1 &= ~TIM_CR1_DIR;
	
	TIM4->CR2 &= ~TIM_CR2_MMS;
	TIM4->CR2 |= TIM_CR2_MMS_2;
	
	TIM4->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM4->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	
	TIM4->PSC = 7;
	TIM4->ARR = 1999;
	TIM4->CCR1 = 500;
	
	TIM4->CCER |= TIM_CCER_CC1E;
	TIM4->EGR |= TIM_EGR_UG;
	TIM4->CR1 |= TIM_CR1_CEN;
	
	NVIC_SetPriority(TIM4_IRQn, 0);
	NVIC_EnableIRQ(TIM4_IRQn);
}
