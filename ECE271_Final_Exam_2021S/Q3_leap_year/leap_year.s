	AREA    fun, CODE, READONLY, ALIGN=3
		
leap_year PROC
	EXPORT leap_year
		MOV r2, #400	; Mod year by 400
		UDIV r3, r0, r2
		MLS r3, r2, r3, r0
		CBZ r3, LEAP
		
		MOV r2, #4		; Mod 4 
		UDIV r3, r0, r2
		MLS r3, r2, r3, r0
		CBNZ r3, NOLEAP
		MOV r2, #100	; Mod 100
		UDIV r3, r0, r2
		MLS r3, r2, r3, r0
		CBZ r3, NOLEAP
		
LEAP
	MOV r0, #1
	B SKIP
NOLEAP
	MOV r0, #0
SKIP
	BX LR
	ENDP
	END