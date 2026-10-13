	extrn	.begin,.chl,.swt
	extrn	csave,cret,.move
	PUBLIC clear_sc_
clear_sc_:	lxi d,.2
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
	RET
.2 EQU 0
.1:
	DB 37,99,0
	PUBLIC set_curs_
set_curs_:	lxi d,.4
	call csave
	LXI H,10-.4
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	LXI H,31
	DAD D
	PUSH H
	LXI H,10-.4
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	LXI H,31
	DAD D
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.3+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	RET
.4 EQU 0
.3:
	DB 37,99,61,37,99,37,99,0
	PUBLIC test01_
test01_:	lxi d,.6
	call csave
	CALL clear_sc_
	LXI H,1
	XCHG
	LXI H,2-.6
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	JMP .8
.7:
	LXI H,2-.6
	DAD SP
	PUSH H
	MOV A,M
	INX H
	MOV H,M
	MOV L,A
	INX H
	XCHG
	POP H
	MOV M,E
	INX H
	MOV M,D
.8:
	LXI H,2-.6
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	LXI H,24
	CALL .le
	JZ .9
	LXI H,1
	PUSH H
	LXI H,4-.6
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	CALL set_curs_
	POP D
	POP D
	LXI H,2-.6
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.5+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	POP D
	JMP .7
.9:
	LXI H,20
	PUSH H
	LXI H,5
	PUSH H
	CALL set_curs_
	POP D
	POP D
	LXI H,.5+6
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	RET
.6 EQU -2
.5:
	DB 37,48,50,100,58,0,84,104,105,115,32,115,104,111,117
	DB 108,100,32,98,101,32,97,116,32,114,111,119,32,53,44
	DB 32,99,111,108,117,109,110,32,50,48,0
	PUBLIC test02_
test02_:	lxi d,.11
	call csave
	CALL clear_sc_
	RET
.11 EQU 0
	PUBLIC wait_nex_
wait_nex_:	lxi d,.12
	call csave
	LXI H,25
	PUSH H
	LXI H,24
	PUSH H
	CALL set_curs_
	POP D
	POP D
	LXI H,.10+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,Cbuffs_
	PUSH H
	CALL agetc_
	POP D
	CALL clear_sc_
	RET
.12 EQU 0
.10:
	DB 80,114,101,115,115,32,97,110,121,32,107,101,121,32,116
	DB 111,32,99,111,110,116,105,110,117,101,32,46,46,46,0
	PUBLIC main_
main_:	lxi d,.14
	call csave
	CALL test01_
	CALL wait_nex_
	CALL test02_
	CALL wait_nex_
	RET
.14 EQU 0
	extrn	agetc_
	extrn	fprintf_
	extrn	Cbuffs_
	extrn	.le
	END
