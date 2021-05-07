	AREA    fun, CODE, READONLY, ALIGN=3
		
min PROC
	PUSH {LR}
	EXPORT min
		VCMPE.F32 s0, s1		; Compare s0 with s1
		VMRS APSR_nzcv, FPSCR
		VMOVGT.F32 s0, s1		; Mov it if its  greater
		VCMPE.F32 s0, s2		; Compare previous one with the second val
		VMRS APSR_nzcv, FPSCR
		VMOVGT.F32 s0, s2		; Mov it if its greater
		VCMPE.F32 s0, s3		; compare the previous min with current
		VMRS APSR_nzcv, FPSCR
		VMOVGT.F32 s0, s3		; MOV if its true
	POP {LR}
	BX LR
	ENDP
	END