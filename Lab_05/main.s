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
	IMPORT INIT_STEPPER					; stepper_motor.s
	IMPORT HALF_STEP					; stepper_motor.s
	IMPORT System_Clock_Init
	IMPORT I2C_GPIO_init
	IMPORT ssd1306_Init
	IMPORT ssd1306_Fill
	IMPORT ssd1306_WriteChar
	IMPORT ssd1306_UpdateScreen
	IMPORT simple_arc
	
	AREA    main, CODE, READONLY
	EXPORT	__main				; make __main visible to linker
	ENTRY			


;---------------------------------------------------
; Main Function
__main	PROC
	BL System_Clock_Init
;	BL I2C_GPIO_init
;	BL ssd1306_Init
;	MOV r0, #0x00
;	BL ssd1306_Fill
	BL INIT_STEPPER 			; Uses PC.5, PC.6, PC.8 and PC.9
;	MOV r3, #0
;	
;WHILE_MAIN
;	CMP r3, #360
;	MOVEQ r3, #0
;	ADD r3, r3, #1
;	MOV r0, r3
;	BL simple_arc
;	MOV r0, #2
;	MOV r1, #12
;	MUL r0, r0, r1
;	BL HALF_STEP
;	B WHILE_MAIN
	MOV r0, #4096
	BL HALF_STEP
stop B stop

	ENDP
;---------------------------------------------------
		
	ALIGN			

	AREA    myData, DATA, READWRITE
	ALIGN
array	DCD   1, 2, 3, 4
	END