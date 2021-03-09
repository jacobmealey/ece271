; A stinky delay function. 
; Author: Jacob Mealey <jacob.mealey@maine.edu>
	INCLUDE core_cm4_constants.s	
	INCLUDE stm32l476xx_constants.s    

	EXPORT DELAY

								; Delay is done as dead loop of some many ticks
DELAY PROC
	PUSH {r6}
	MOV r6, #0 					; Set r6 zero
WHILE_DELAY 					; While r6 does not equal r0 
		ADD r6, r6, #1			; Increment r6
		CMP r0, r6
		BNE WHILE_DELAY
		POP {r6}
	BX lr						; return to main
	ENDP
	END
