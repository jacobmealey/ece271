	AREA    fun, CODE, READONLY, ALIGN=3
		
leap_year PROC
	EXPORT leap_year
		MOV r1, r0 MOD 400 
	ENDP
	END