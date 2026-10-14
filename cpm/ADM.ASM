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
	LXI H,10
	PUSH H
	LXI H,1
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,8-.12
	DAD SP
	mov a,m
	inx h
	ora m
	JZ .13
	LXI H,8-.12
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.11+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	POP D
.13:
	RET
.12 EQU 0
.11:
	DB 65,68,77,45,51,65,32,99,111,110,116,114,111,108,32
	DB 115,101,113,117,101,110,99,101,32,116,101,115,116,101,114
	DB 58,32,37,115,0
	PUBLIC test01_
test01_:	lxi d,.15
	call csave
	CALL cls_
	LXI H,8-.15
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	CALL header_
	POP D
	LXI H,1
	XCHG
	LXI H,2-.15
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	JMP .17
.16:
	LXI H,2-.15
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
.17:
	LXI H,2-.15
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	LXI H,24
	CALL .le
	JZ .18
	LXI H,1
	PUSH H
	LXI H,4-.15
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	CALL cursor_
	POP D
	POP D
	LXI H,2-.15
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.14+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	POP D
	JMP .16
.18:
	LXI H,20
	PUSH H
	LXI H,5
	PUSH H
	CALL cursor_
	POP D
	POP D
	LXI H,.14+6
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	RET
.15 EQU -2
.14:
	DB 37,48,50,100,58,0,84,104,105,115,32,115,104,111,117
	DB 108,100,32,98,101,32,97,116,32,114,111,119,32,53,44
	DB 32,99,111,108,117,109,110,32,50,48,0
	PUBLIC test02_
test02_:	lxi d,.20
	call csave
	CALL cls_
	LXI H,8-.20
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	CALL header_
	POP D
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
	LXI H,.19+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.19+7
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
	LXI H,.19+21
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.19+28
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
	LXI H,.19+42
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.19+49
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
	LXI H,.19+70
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.19+77
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
	LXI H,.19+111
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	RET
.20 EQU 0
.19:
	DB 37,99,37,99,37,100,0,78,111,114,109,97,108,101,114
	DB 32,84,101,120,116,0,37,99,37,99,37,100,0,73,110
	DB 118,101,114,115,101,114,32,84,101,120,116,0,37,99,37
	DB 99,37,100,0,85,110,116,101,114,115,116,114,105,99,104
	DB 101,110,101,114,32,84,101,120,116,0,37,99,37,99,37
	DB 99,0,85,110,116,101,114,115,116,114,105,99,104,101,110
	DB 101,114,32,117,110,100,32,73,110,118,101,114,115,101,114
	DB 32,84,101,120,116,0,37,99,37,99,37,100,0
	PUBLIC test03_
test03_:	lxi d,.22
	call csave
	CALL cls_
	LXI H,8-.22
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	CALL header_
	POP D
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
	LXI H,.21+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.21+7
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
	LXI H,.21+23
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.21+30
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
	LXI H,.21+46
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.21+53
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
	LXI H,.21+76
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.21+83
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
	LXI H,.21+113
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	RET
.22 EQU 0
.21:
	DB 37,99,37,99,37,100,0,78,111,114,109,97,108,32,66
	DB 108,105,110,107,101,110,100,0,37,99,37,99,37,100,0
	DB 73,110,118,101,114,115,32,66,108,105,110,107,101,110,100
	DB 0,37,99,37,99,37,99,0,85,110,116,101,114,115,116
	DB 114,105,99,104,101,110,32,66,108,105,110,107,101,110,100
	DB 0,37,99,37,99,37,99,0,85,110,116,101,114,115,116
	DB 114,105,99,104,101,110,32,73,110,118,101,114,115,32,66
	DB 108,105,110,107,101,110,100,0,37,99,37,99,37,100,0
	PUBLIC test04_
test04_:	lxi d,.24
	call csave
	CALL cls_
	LXI H,8-.24
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	CALL header_
	POP D
	LXI H,5
	XCHG
	LXI H,-2-.24
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,5
	XCHG
	LXI H,0-.24
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,0-.24
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,0-.24
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
	LXI H,2-.24
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	JMP .26
.25:
	LXI H,2-.24
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
.26:
	LXI H,2-.24
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	LXI H,96
	CALL .lt
	JZ .27
	LXI H,0-.24
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,0-.24
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
	LXI H,.23+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,2-.24
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.23+7
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
	LXI H,.23+14
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,2-.24
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.23+21
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
	LXI H,.23+24
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.23+31
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,-2-.24
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
	LXI H,2-.24
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	XCHG
	INX H
	LXI D,8
	XCHG
	CALL .rm
	JNZ .28
	LXI H,5
	XCHG
	LXI H,-2-.24
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,8
	XCHG
	LXI H,0-.24
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
.28:
	JMP .25
.27:
	RET
.24 EQU -6
.23:
	DB 37,99,37,99,37,99,0,37,48,51,100,61,91,0,37
	DB 99,37,99,37,99,0,37,99,0,37,99,37,99,37,99
	DB 0,93,0
	PUBLIC test05_
test05_:	lxi d,.30
	call csave
	CALL cls_
	LXI H,8-.30
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	CALL header_
	POP D
	LXI H,5
	XCHG
	LXI H,-2-.30
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,5
	XCHG
	LXI H,0-.30
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,0-.30
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,0-.30
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
	LXI H,2-.30
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	JMP .32
.31:
	LXI H,2-.30
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
.32:
	LXI H,2-.30
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	LXI H,96
	CALL .lt
	JZ .33
	LXI H,0-.30
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,0-.30
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
	LXI H,.29+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,2-.30
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.29+7
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	POP D
	LXI H,82
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.29+14
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,2-.30
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.29+21
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
	LXI H,.29+24
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.29+31
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,-2-.30
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
	LXI H,2-.30
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	XCHG
	INX H
	LXI D,8
	XCHG
	CALL .rm
	JNZ .34
	LXI H,5
	XCHG
	LXI H,-2-.30
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,8
	XCHG
	LXI H,0-.30
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
.34:
	JMP .31
.33:
	RET
.30 EQU -6
.29:
	DB 37,99,37,99,37,99,0,37,48,51,100,61,91,0,37
	DB 99,37,99,37,99,0,37,99,0,37,99,37,99,37,99
	DB 0,93,0
	PUBLIC test06_
test06_:	lxi d,.36
	call csave
	CALL cls_
	LXI H,8-.36
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	CALL header_
	POP D
	LXI H,5
	XCHG
	LXI H,-2-.36
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,5
	XCHG
	LXI H,0-.36
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,0-.36
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,0-.36
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
	LXI H,2-.36
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	JMP .38
.37:
	LXI H,2-.36
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
.38:
	LXI H,2-.36
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	LXI H,96
	CALL .lt
	JZ .39
	LXI H,0-.36
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,0-.36
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
	LXI H,.35+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,2-.36
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.35+7
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	POP D
	LXI H,84
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.35+14
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,2-.36
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.35+21
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
	LXI H,.35+24
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.35+31
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,-2-.36
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
	LXI H,2-.36
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	XCHG
	INX H
	LXI D,8
	XCHG
	CALL .rm
	JNZ .40
	LXI H,5
	XCHG
	LXI H,-2-.36
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,8
	XCHG
	LXI H,0-.36
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
.40:
	JMP .37
.39:
	RET
.36 EQU -6
.35:
	DB 37,99,37,99,37,99,0,37,48,51,100,61,91,0,37
	DB 99,37,99,37,99,0,37,99,0,37,99,37,99,37,99
	DB 0,93,0
	PUBLIC test07_
test07_:	lxi d,.42
	call csave
	CALL cls_
	LXI H,8-.42
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	CALL header_
	POP D
	LXI H,5
	XCHG
	LXI H,-2-.42
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,5
	XCHG
	LXI H,0-.42
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,0-.42
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,0-.42
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
	LXI H,2-.42
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	JMP .44
.43:
	LXI H,2-.42
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
.44:
	LXI H,2-.42
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	LXI H,96
	CALL .lt
	JZ .45
	LXI H,0-.42
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,0-.42
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
	LXI H,.41+0
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,2-.42
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.41+7
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	POP D
	LXI H,86
	PUSH H
	LXI H,71
	PUSH H
	LXI H,27
	PUSH H
	LXI H,.41+14
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,2-.42
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	PUSH D
	LXI H,.41+21
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
	LXI H,.41+24
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	XCHG
	LXI H,10
	DAD SP
	SPHL
	LXI H,.41+31
	PUSH H
	LXI H,Cbuffs_+11
	PUSH H
	CALL fprintf_
	POP D
	POP D
	LXI H,-2-.42
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
	LXI H,2-.42
	DAD SP
	MOV E,M
	INX H
	MOV D,M
	XCHG
	INX H
	LXI D,8
	XCHG
	CALL .rm
	JNZ .46
	LXI H,5
	XCHG
	LXI H,-2-.42
	DAD SP
	MOV M,E
	INX H
	MOV M,D
	LXI H,8
	XCHG
	LXI H,0-.42
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
.46:
	JMP .43
.45:
	RET
.42 EQU -6
.41:
	DB 37,99,37,99,37,99,0,37,48,51,100,61,91,0,37
	DB 99,37,99,37,99,0,37,99,0,37,99,37,99,37,99
	DB 0,93,0
	PUBLIC main_
main_:	lxi d,.48
	call csave
	LXI H,.47+0
	PUSH H
	CALL test01_
	POP D
	CALL wait_
	LXI H,.47+19
	PUSH H
	CALL test02_
	POP D
	CALL wait_
	LXI H,.47+37
	PUSH H
	CALL test03_
	POP D
	CALL wait_
	LXI H,.47+55
	PUSH H
	CALL test04_
	POP D
	CALL wait_
	LXI H,.47+69
	PUSH H
	CALL test05_
	POP D
	CALL wait_
	LXI H,.47+90
	PUSH H
	CALL test06_
	POP D
	CALL wait_
	LXI H,.47+112
	PUSH H
	CALL test07_
	POP D
	CALL wait_
	RET
.48 EQU 0
.47:
	DB 99,117,114,115,111,114,32,112,111,115,105,116,105,111,110
	DB 105,110,103,0,116,101,120,116,32,97,116,116,114,105,98
	DB 117,116,101,115,32,49,0,116,101,120,116,32,97,116,116
	DB 114,105,98,117,116,101,115,32,50,0,103,114,97,112,104
	DB 105,99,115,32,109,111,100,101,0,103,114,97,112,104,105
	DB 99,115,32,98,108,105,110,107,121,32,109,111,100,101,0
	DB 103,114,97,112,104,105,99,115,32,105,110,118,101,114,115
	DB 101,32,109,111,100,101,0,103,114,97,112,104,105,99,115
	DB 32,105,110,118,101,114,115,101,32,98,108,105,110,107,121
	DB 32,109,111,100,101,0
	extrn	agetc_
	extrn	fprintf_
	extrn	Cbuffs_
	extrn	.lt
	extrn	.le
	extrn	.rm
	END
