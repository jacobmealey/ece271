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
; Green LED <--> PA.5
LED_PIN	EQU	5
MOTOR_A_PIN EQU 5
MOTOR_NA_PIN EQU 6
MOTOR_B_PIN EQU 8
MOTOR_NB_PIN EQU 9
	
	AREA    main, CODE, READONLY
	EXPORT	__main				; make __main visible to linker
	ENTRY			
;---------------------------------------------------
; Main Function
__main	PROC
		
	BL INIT_STEPPER
WHILE 
	MOV r0, #1000
	BL HALF_STEP
stop B stop

	ENDP
;---------------------------------------------------
		
	ALIGN			

	AREA    myData, DATA, READWRITE
	ALIGN
array	DCD   1, 2, 3, 4
	END