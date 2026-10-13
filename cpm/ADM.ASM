	extrn	.begin,.chl,.swt
	extrn	csave,cret,.move
	PUBLIC cls_
cls_:	lxi d,.2
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
	PUBLIC cursor_
cursor_:	lxi d,.4
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
	PUBLIC t_bg_
t_bg_:	lxi d,.6
	call csave
	LXI H,41
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.5+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,8
	DAD SP
	SPHL
	RET
.6 EQU 0
.5:
	DB 37,99,37,99,0
	PUBLIC t_fg_
t_fg_:	lxi d,.8
	call csave
	LXI H,40
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.7+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,8
	DAD SP
	SPHL
	RET
.8 EQU 0
.7:
	DB 37,99,37,99,0
	PUBLIC wait_
wait_:	lxi d,.10
	call csave
	LXI H,25
	PUSH H
	LXI H,24
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,.9+0
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
	RET
.10 EQU 0
.9:
	DB 80,114,101,115,115,32,97,110,121,32,107,101,121,32,116
	DB 111,32,99,111,110,116,105,110,117,101,32,46,46,46,0
	PUBLIC header_
header_:	lxi d,.12
	call csave
	LXI H,26
	PUSH H
	LXI H,1
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,.11+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	RET
.12 EQU 0
.11:
	DB 65,68,77,45,51,65,32,99,111,110,116,114,111,108,32
	DB 115,101,113,117,101,110,99,101,32,116,101,115,116,101,114
	DB 0
	PUBLIC test01_
test01_:	lxi d,.14
	call csave
	CALL cls_
	CALL header_
	LXI H,1
	XCHG
	LXI H,2-.14
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	JMP .16
.15:
	LXI H,2-.14
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
.16:
	LXI H,2-.14
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	LXI H,24
	CALL .le
	JZ .17
	LXI H,1
	PUSH H
	LXI H,4-.14
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	CALL cursor_
	POP D
	POP D
	LXI H,2-.14
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.13+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	POP D
	JMP .15
.17:
	LXI H,20
	PUSH H
	LXI H,5
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,.13+6
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	RET
.14 EQU -2
.13:
	DB 37,48,50,100,58,0,84,104,105,115,32,115,104,111,117
	DB 108,100,32,98,101,32,97,116,32,114,111,119,32,53,44
	DB 32,99,111,108,117,109,110,32,50,48,0
	PUBLIC test02_
test02_:	lxi d,.19
	call csave
	CALL cls_
	CALL header_
	LXI H,5
	PUSH H
	LXI H,3
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,0
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.18+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.18+7
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,15
	PUSH H
	LXI H,5
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,4
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.18+21
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.18+28
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,25
	PUSH H
	LXI H,7
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,8
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.18+42
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.18+49
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,35
	PUSH H
	LXI H,9
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,60
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.18+70
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.18+77
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,0
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.18+111
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	RET
.19 EQU 0
.18:
	DB 37,99,37,99,37,100,0,78,111,114,109,97,108,101,114
	DB 32,84,101,120,116,0,37,99,37,99,37,100,0,73,110
	DB 118,101,114,115,101,114,32,84,101,120,116,0,37,99,37
	DB 99,37,100,0,85,110,116,101,114,115,116,114,105,99,104
	DB 101,110,101,114,32,84,101,120,116,0,37,99,37,99,37
	DB 99,0,85,110,116,101,114,115,116,114,105,99,104,101,110
	DB 101,114,32,117,110,100,32,73,110,118,101,114,115,101,114
	DB 32,84,101,120,116,0,37,99,37,99,37,100,0
	PUBLIC test03_
test03_:	lxi d,.21
	call csave
	CALL cls_
	CALL header_
	LXI H,5
	PUSH H
	LXI H,3
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,2
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.20+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.20+7
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,5
	PUSH H
	LXI H,5
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,6
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.20+23
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.20+30
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,5
	PUSH H
	LXI H,7
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,58
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.20+46
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.20+53
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,5
	PUSH H
	LXI H,9
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,62
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.20+76
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.20+83
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,0
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.20+113
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	RET
.21 EQU 0
.20:
	DB 37,99,37,99,37,100,0,78,111,114,109,97,108,32,66
	DB 108,105,110,107,101,110,100,0,37,99,37,99,37,100,0
	DB 73,110,118,101,114,115,32,66,108,105,110,107,101,110,100
	DB 0,37,99,37,99,37,99,0,85,110,116,101,114,115,116
	DB 114,105,99,104,101,110,32,66,108,105,110,107,101,110,100
	DB 0,37,99,37,99,37,99,0,85,110,116,101,114,115,116
	DB 114,105,99,104,101,110,32,73,110,118,101,114,115,32,66
	DB 108,105,110,107,101,110,100,0,37,99,37,99,37,100,0
	PUBLIC test04_
test04_:	lxi d,.23
	call csave
	CALL cls_
	CALL header_
	LXI H,5
	XCHG
	LXI H,-2-.23
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,5
	XCHG
	LXI H,0-.23
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,0-.23
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,0-.23
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	CALL cursor_
	POP D
	POP D
	LXI H,32
	XCHG
	LXI H,2-.23
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	JMP .25
.24:
	LXI H,2-.23
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
.25:
	LXI H,2-.23
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	LXI H,96
	CALL .lt
	JZ .26
	LXI H,0-.23
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,0-.23
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	CALL cursor_
	POP D
	POP D
	LXI H,48
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.22+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,2-.23
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.22+7
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	POP D
	LXI H,80
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.22+14
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,2-.23
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.22+21
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	POP D
	LXI H,48
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.22+24
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.22+31
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,-2-.23
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
	LXI H,2-.23
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	XCHG
	INX H
	LXI D,8
	XCHG
	CALL .rm
	JNZ .27
	LXI H,5
	XCHG
	LXI H,-2-.23
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,8
	XCHG
	LXI H,0-.23
	DAD SP
	PUSH H
	MOV A,M
	INX H
	MOV H,M
	MOV L,A
	DAD D
	XCHG
	POP H
	MOV M,E
	INX H
	MOV M,D
.27:
	JMP .24
.26:
	RET
.23 EQU -6
.22:
	DB 37,99,37,99,37,99,0,37,48,51,100,61,91,0,37
	DB 99,37,99,37,99,0,37,99,0,37,99,37,99,37,99
	DB 0,93,0
	PUBLIC main_
main_:	lxi d,.29
	call csave
	CALL test01_
	CALL wait_
	CALL test02_
	CALL wait_
	CALL test03_
	CALL wait_
	CALL test04_
	CALL wait_
	RET
.29 EQU 0
	extrn	agetc_
	extrn	fprintf_
	extrn	Cbuffs_
	extrn	.lt
	extrn	.le
	extrn	.rm
	END
