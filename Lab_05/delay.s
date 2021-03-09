	INCLUDE core_cm4_constants.s		; Load Constant Definitions
	INCLUDE stm32l476xx_constants.s    

	EXPORT DELAY

DELAY PROC
		MOV r6, #0 				; Set r6 zero
WHILE_DELAY 					; While r6 does not equal r0 
		ADD r6, r6, #1			; Increment r6
		CMP r0, r6
		BNE WHILE_DELAY
 	BX lr						; return to main
	ENDP
	END
