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
	
	; Set GPIOC input;
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_MODER]
	EOR r1, r0, #(3 << (2 * BUTTON_PIN))
	AND r1, r0, #(~(3 << (2 * BUTTON_PIN)))
	STR r1, [r0, #GPIO_MODER]

	LDR r0, =GPIOA_BASE

	MOV r8, #100 				; MAX duty cycle
	MOV r7, #0					; Current duty cycle
WHILE
	; -------------	PWM IN ASSEMBLY	--------------------- 		
	ADD r7, r7, #1				; Add 1 to current duty cycle
	CMP r7, r8					; Are we at max duty cycle?
	MOVEQ r7, #1				; if we are reset to 0
		
	MOV r0, #50			
	MUL r0, r0, r7				; Multiply r0 by the duty cycle

	BL DELAY					; Pass r0 to delay to dead loop that many ticks
	BL LED_OFF

	SUB r9, r8, r7				; Subtract the current duty cycle from the max duty
	MOV r0, #50					; cycle to give time low.
	MUL r0, r9, r9

	BL DELAY					; r0 is passed to delay
	BL LED_ON
	
	
;	LDR r0, =GPIOC_BASE			; Go to GPIOC
	; Get button pin 			
;	LDR r2, [r0, #GPIO_IDR] 	; setup r2 as GPIOC->IDR
	; shift to LED location
;	LSR r2, r2, #BUTTON_PIN		; set r2 to have first bit be button value

	
	
	
;	CMP r2, #(0x01)				; check if button is pressed	
;	BNE LEDTOGGLE				; if they aren't equal go to LED
;	B WHILE						; else go to beginnging of loop

;	BL LED_TOGGLE


	B WHILE 					; Just back to beginning of while
 
  

	ENDP

LED_ON PROC
	MOV r2, r0					
	LDR r0, =GPIOA_BASE
	LDR r1, [r0, #GPIO_ODR]
	ORR r1, r1, #(1<<LED_PIN) 		
	STR r1, [r0, #GPIO_ODR]
	BX lr
	ENDP

LED_OFF PROC
	LDR r0, =GPIOA_BASE
	LDR r1, [r0, #GPIO_ODR]
	AND r1, r1, #(~(1<<LED_PIN)) 		
	STR r1, [r0, #GPIO_ODR]
	BX lr
	ENDP
		
LED_TOGGLE PROC
	LDR r0, =GPIOA_BASE
	LDR r1, [r0, #GPIO_ODR]
	EOR r1, r1, #(1<<LED_PIN) 	; Toggle LED 
	STR r1, [r0, #GPIO_ODR]
	BX lr
	ENDP

DELAY PROC
		MOV r6, #0 				; Set r6 zero
WHILE_DELAY 					; While r6 does not equal r0 
		ADD r6, r6, #1			; Increment r6
		CMP r0, r6
		BNE WHILE_DELAY
 	BX lr						; return to main
	ENDP
		
	ALIGN			

	AREA    myData, DATA, READWRITE
	ALIGN
array	DCD   1, 2, 3, 4
	END