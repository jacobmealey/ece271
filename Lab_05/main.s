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
MOTOR_A_PIN EQU 5
MOTOR_NA_PIN EQU 6
MOTOR_B_PIN EQU 8
MOTOR_NB_PIN EQU 9
	
	AREA    main, CODE, READONLY
	EXPORT	__main				; make __main visible to linker
	ENTRY			
				
__main	PROC
		
	BL INIT_STEPPER
WHILE 
	MOV r0, #1000
	BL HALF_STEP
stop B stop

	ENDP
INIT_STEPPER PROC
	; Enable the clock to GPIO Port A	
	LDR r0, =RCC_BASE
	LDR r1, [r0, #RCC_AHB2ENR]
	ORR r1, r1, #RCC_AHB2ENR_GPIOCEN
	STR r1, [r0, #RCC_AHB2ENR]

	; MODE: 00: Input mode, 01: General purpose output mode
    ;       10: Alternate function mode, 11: Analog mode (reset state)
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_MODER]
	AND r1, r1, #(~(3<<(2*MOTOR_A_PIN)))
	ORR r1, r1, #(1<<(2*MOTOR_A_PIN))
	AND r1, r1, #(~(3<<(2*MOTOR_NA_PIN)))
	ORR r1, r1, #(1<<(2*MOTOR_NA_PIN))
	AND r1, r1, #(~(3<<(2*MOTOR_B_PIN)))
	ORR r1, r1, #(1<<(2*MOTOR_B_PIN))
	AND r1, r1, #(~(3<<(2*MOTOR_NB_PIN)))
	ORR r1, r1, #(1<<(2*MOTOR_NB_PIN))
	STR r1, [r0, #GPIO_MODER]
	BX lr
	endp
		
FULL_STEP PROC
	MOV r4, #0x56A9				; Each hex digit is 1 sequence in the fullstep
	MOV r5, #0 					; the current 4 bit section to look at
								; this will be used for shifting
	MOV r8, r0					; r8 has max steps 
	mov r10, #0					; r10 has current steps
	
FS_MOTOR_LOOP						; Start motor loop

	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_ODR]

	LSR r6, r4, r5				; Shift four over r5 amount of bits 
	AND r6, r6, #0xF			; Mask so we only get the first 4 bits :)
	
	ADD r5, r5, #4				; increment r5 
	CMP r5, #16
	MOVEQ r5, #0
	;AND r7, 
	AND r1, r1, #~(1<< MOTOR_A_PIN)
	AND r1, r1, #~(1<< MOTOR_NA_PIN)
	AND r1, r1, #~(1<< MOTOR_B_PIN)
	AND r1, r1, #~(1<< MOTOR_NB_PIN)
	STR r1, [r0, #GPIO_ODR]
	
	
	LSR r7, r6, #3
	AND r7, r7, #0x1
	LSL r7, r7, #MOTOR_A_PIN
	ORR r1, r1, r7
	
	LSR r7, r6, #2
	AND r7, r7, #0x1
	LSL r7, r7, #MOTOR_NA_PIN
	ORR r1, r1, r7
	
	LSR r7, r6, #1
	AND r7, r7, #0x1
	LSL r7, r7, #MOTOR_B_PIN
	ORR r1, r1, r7
	
	AND r7, r7, #0x1
	LSL r7, r7, #MOTOR_NB_PIN
	ORR r1, r1, r7
	
	STR r1, [r0, #GPIO_ODR]
	
	MOV r0, #(1<<11)
	BL DELAY
	
	ADD r10, r10, #1
	CMP r8, r10
	BGE FS_MOTOR_LOOP
	
	LDR r0, =GPIOC_BASE					; Reset all motor pins
	LDR r1, [r0, #GPIO_ODR]
	AND r1, r1, #~(1<< MOTOR_A_PIN)		
	AND r1, r1, #~(1<< MOTOR_NA_PIN)
	AND r1, r1, #~(1<< MOTOR_B_PIN)
	AND r1, r1, #~(1<< MOTOR_NB_PIN)
	STR r1, [r0, #GPIO_ODR]
	BX lr
	ENDP

HALF_STEP PROC

	; MOV r4 #0x2A891546			; Each hex digit is 1 sequence in the fullstep
	MOV r4, #(0x2A << 24)			; Each hex digit is 1 sequence in the fullstep
	ORR r4, #(0x89 << 16)
	ORR r4, #(0x15 << 8)			; This is kind of a yucky way to do it but screw it.
	ORR r4, #0x46
	MOV r5, #0 					; the current 4 bit section to look at
								; this will be used for shifting
	MOV r8, r0					; r8 has max steps 
	MOV r7, #2
	MUL r10, r8, r7
	MOV r10, #0					; r10 has current steps
	
HS_MOTOR_LOOP						; Start motor loop
	
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_ODR]

	LSR r6, r4, r5				; Shift four over r5 amount of bits 
	AND r6, r6, #0xF			; Mask so we only get the first 4 bits :)
	
	ADD r5, r5, #4				; increment r5 
	CMP r5, #32
	MOVEQ r5, #0
	;AND r7, 
	
	AND r1, r1, #~(1<< MOTOR_A_PIN)		
	AND r1, r1, #~(1<< MOTOR_NA_PIN)
	AND r1, r1, #~(1<< MOTOR_B_PIN)
	AND r1, r1, #~(1<< MOTOR_NB_PIN)
	STR r1, [r0, #GPIO_ODR]
	
	LSR r7, r6, #3
	AND r7, r7, #0x1
	LSL r7, r7, #MOTOR_A_PIN
	ORR r1, r1, r7
	
	LSR r7, r6, #2
	AND r7, r7, #0x1
	LSL r7, r7, #MOTOR_NA_PIN
	ORR r1, r1, r7
	
	LSR r7, r6, #1
	AND r7, r7, #0x1
	LSL r7, r7, #MOTOR_B_PIN
	ORR r1, r1, r7
	
	AND r7, r7, #0x1
	LSL r7, r7, #MOTOR_NB_PIN
	ORR r1, r1, r7
	
	STR r1, [r0, #GPIO_ODR]
	
	MOV r0, #(1<<10)
	BL DELAY
	
	ADD r10, r10, #1
	CMP r8, r10
	BGE HS_MOTOR_LOOP
	
	LDR r0, =GPIOC_BASE					; Reset all motor pins
	LDR r1, [r0, #GPIO_ODR]
	AND r1, r1, #~(1<< MOTOR_A_PIN)		
	AND r1, r1, #~(1<< MOTOR_NA_PIN)
	AND r1, r1, #~(1<< MOTOR_B_PIN)
	AND r1, r1, #~(1<< MOTOR_NB_PIN)
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