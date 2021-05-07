	AREA    fun, CODE, READONLY, ALIGN=3
	
STUDENT_C1 EQU 0
STUDENT_SCORE EQU 4
STUDENT_C2 EQU 8	
STUDENT_SIZE EQU 12
find_avg_score PROC
	push {lr, r4, r5}
	MOV r3, r0
	MOV r4, #0 ; incrementer 
	MOV r5, #0 ; sum of all scores
	EXPORT find_avg_score
WHILE	; Loop through the array to sum scores
		LDR r2, [r3, #STUDENT_SCORE] ; load Score into r2
		ADD r3, r3, #STUDENT_SIZE	; increment
		ADD r5, r5, r2 ;add whats in r3 to our r5
		ADD r4, #1	; Increment r4
		CMP r4, r1	; compare r4 with the max
	BLT WHILE
	UDIV r0, r5, r1 ; divide sum by max length 
	pop {pc, r4, r5}
	BX lr
	ENDP
	END