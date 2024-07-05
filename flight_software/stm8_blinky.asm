;--------------------------------------------------------
; File Created by SDCC : free open source ISO C Compiler 
; Version 4.4.0 #14620 (Linux)
;--------------------------------------------------------
	.module stm8_blinky
	.optsdcc -mstm8
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _GPIO_WriteReverse
	.globl _GPIO_WriteHigh
	.globl _GPIO_Init
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area DATA
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area INITIALIZED
;--------------------------------------------------------
; Stack segment in internal ram
;--------------------------------------------------------
	.area SSEG
__start__stack:
	.ds	1

;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area DABS (ABS)

; default segment ordering for linker
	.area HOME
	.area GSINIT
	.area GSFINAL
	.area CONST
	.area INITIALIZER
	.area CODE

;--------------------------------------------------------
; interrupt vector
;--------------------------------------------------------
	.area HOME
__interrupt_vect:
	int s_GSINIT ; reset
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area HOME
	.area GSINIT
	.area GSFINAL
	.area GSINIT
	call	___sdcc_external_startup
	tnz	a
	jreq	__sdcc_init_data
	jp	__sdcc_program_startup
__sdcc_init_data:
; stm8_genXINIT() start
	ldw x, #l_DATA
	jreq	00002$
00001$:
	clr (s_DATA - 1, x)
	decw x
	jrne	00001$
00002$:
	ldw	x, #l_INITIALIZER
	jreq	00004$
00003$:
	ld	a, (s_INITIALIZER - 1, x)
	ld	(s_INITIALIZED - 1, x), a
	decw	x
	jrne	00003$
00004$:
; stm8_genXINIT() end
	.area GSFINAL
	jp	__sdcc_program_startup
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area HOME
	.area HOME
__sdcc_program_startup:
	jp	_main
;	return from main will return to caller
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE
	G$main$0$0 ==.
	C$stm8_blinky.c$8$0_0$358 ==.
;	./stm8_blinky.c: 8: void main(void) 
; genLabel
;	-----------------------------------------
;	 function main
;	-----------------------------------------
;	Register assignment is optimal.
;	Stack space usage: 0 bytes.
_main:
	C$stm8_blinky.c$12$1_0$358 ==.
;	./stm8_blinky.c: 12: Led_Init;
; genIPush
	push	#0xe0
; genSend
	ld	a, #0x02
; genSend
	ldw	x, #0x500f
; genCall
	call	_GPIO_Init
	C$stm8_blinky.c$15$1_0$358 ==.
;	./stm8_blinky.c: 15: Led_ON;
; genSend
	ld	a, #0x02
; genSend
	ldw	x, #0x500f
; genCall
	call	_GPIO_WriteHigh
	C$stm8_blinky.c$18$1_0$358 ==.
;	./stm8_blinky.c: 18: while(1){
; genLabel
00104$:
	C$stm8_blinky.c$20$2_0$359 ==.
;	./stm8_blinky.c: 20: Led_TOG;
; genSend
	ld	a, #0x02
; genSend
	ldw	x, #0x500f
; genCall
	call	_GPIO_WriteReverse
	C$stm8_blinky.c$23$1_0$358 ==.
;	./stm8_blinky.c: 23: for(uint16_t d = 0; d<19000; d++){
; genAssign
	clrw	x
; genLabel
00110$:
; genCast
; genAssign
	ldw	y, x
; genCmp
; genCmpTnz
	cpw	y, #0x4a38
	jrc	00150$
	jp	00104$
00150$:
; skipping generated iCode
	C$stm8_blinky.c$24$1_0$358 ==.
;	./stm8_blinky.c: 24: for(uint8_t c = 0; c<5; c++);
; genAssign
	clr	a
; genLabel
00107$:
; genCmp
; genCmpTnz
	cp	a, #0x05
	jrc	00151$
	jp	00111$
00151$:
; skipping generated iCode
; genPlus
	inc	a
; genGoto
	jp	00107$
; genLabel
00111$:
	C$stm8_blinky.c$23$3_0$360 ==.
;	./stm8_blinky.c: 23: for(uint16_t d = 0; d<19000; d++){
; genPlus
	incw	x
; genGoto
	jp	00110$
; genLabel
00112$:
	C$stm8_blinky.c$27$1_0$358 ==.
;	./stm8_blinky.c: 27: }
; genEndFunction
	C$stm8_blinky.c$27$1_0$358 ==.
	XG$main$0$0 ==.
	ret
	.area CODE
	.area CONST
	.area INITIALIZER
	.area CABS (ABS)
