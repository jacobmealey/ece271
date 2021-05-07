	AREA    fun, CODE, READONLY, ALIGN=3
		
compare64 PROC
	EXPORT compare64
	PUSH {lr}
	CMP r1, r3	; compare upper 32
	BEQ LOWER
	MOVGT r0, #1
	MOVLT r0, #-1
	B SKIP	;We don't need to check lower
LOWER
		CMP r0, r2 	; compare lower 32
		MOVEQ r0, #0
		MOVGT r0, #1
		MOVLT r0, #-1
SKIP
	POP {pc}
	BX lr
	ENDP
	END