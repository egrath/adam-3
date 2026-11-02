	extrn	.begin,.chl,.swt
	extrn	csave,cret,.move
	PUBLIC main_
main_:	lxi d,.2
	call csave
	LXI H,26
	PUSH H
	LXI H,.1+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	POP D
	LXI H,36
	PUSH H
	LXI H,36
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.1+3
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.1+11
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	RET
.2 EQU 0
.1:
	DB 37,99,0,37,99,61,37,99,37,99,0,65,66,67,0
	extrn	fprintf_
	extrn	Cbuffs_
	END
