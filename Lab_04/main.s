;******************** (C) Yifeng ZHU *******************************************
; @file    main.s
; @author  Yifeng Zhu
; @date    May-17-2015
; @note
;           This code is for the book "Embedded Systems with ARM Cortex-M 
;           Microcontrollers in Assembly Language and C, Yifeng Zhu, 
;           ISBN-13: 978-0982692639, ISBN-10: 0982692633
; @attension
;           This code is provided for education purpose. The author shall not be 
;           held liable for any direct, indirect or consequential damages, for any 
;           reason whatever. More information can be found from book website: 
;           http:;www.eece.maine.edu/~zhu/book
;*******************************************************************************


	INCLUDE core_cm4_constants.s		; Load Constant Definitions
	INCLUDE stm32l476xx_constants.s      

; Green LED <--> PA.5
LED_PIN	EQU	5
; Blue Push button <--> PC.13
BUTTON_PIN EQU 13
	
	AREA    main, CODE, READONLY
	EXPORT	__main				; make __main visible to linker
	ENTRY			
				
__main	PROC
		
    ; Enable the clock to GPIO Port A and C
	LDR r0, =RCC_BASE
	LDR r1, [r0, #RCC_AHB2ENR]
	ORR r1, r1, #RCC_AHB2ENR_GPIOAEN
	ORR r1, r1, #RCC_AHB2ENR_GPIOCEN
	STR r1, [r0, #RCC_AHB2ENR]
	
	; MODE: 00: Input mode, 01: General purpose output mode
    ;       10: Alternate function mode, 11: Analog mode (reset state)
	LDR r0, =GPIOA_BASE
	LDR r1, [r0, #GPIO_MODER]
    EOR r1, r1, #(3<<(2*LED_PIN))
    ORR r1, r1, #(1<<(2*LED_PIN))
	STR r1, [r0, #GPIO_MODER]
	
	LDR r1, [r0, #GPIO_ODR]
	ORR r1, r1, r2
	STR r1, [r0, #GPIO_ODR]
	
	; Set GPIOC input;
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_MODER]
	EOR r1, r0, #(3 << (2 * BUTTON_PIN))
	AND r1, r0, #(~(3 << (2 * BUTTON_PIN)))
	LDR r2, [r0, #GPIO_IDR]
	LSR r2, r2, BUTTON_PIN
	LSL r2, r2, LED_PIN
	;ORR r3, r2, #(0x2000)
	;STR r3, [r0, #(GPIO_IDR+BUTTON_PIN)]
	STR r1, [r0, #GPIO_MODER]
	
	B __main
  
stop 	B 		stop     		; dead loop & program hangs here

	ENDP
					
	ALIGN			

	AREA    myData, DATA, READWRITE
	ALIGN
array	DCD   1, 2, 3, 4
	END