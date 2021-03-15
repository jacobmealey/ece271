#include "I2C_gpio.h"

void I2C_GPIO_init(void){

	RCC->AHB2ENR  |=  RCC_AHB2ENR_GPIOBEN;
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
	I2C_Initialization(I2C1);

}
void simple_arc(uint16_t sweep){
	static uint16_t offset = 0;
	offset += sweep;
	if(offset > 360){
		ssd1306_Fill(Black);
		offset = 0;
	}
	ssd1306_DrawArc(SSD1306_WIDTH/2, SSD1306_HEIGHT/2, 15, 0, offset, White);
  ssd1306_UpdateScreen();
	
}

