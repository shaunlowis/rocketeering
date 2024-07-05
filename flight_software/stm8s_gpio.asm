;--------------------------------------------------------
; File Created by SDCC : free open source ISO C Compiler 
; Version 4.4.0 #14620 (Linux)
;--------------------------------------------------------
	.module stm8s_gpio
	.optsdcc -mstm8
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _assert_failed
	.globl _GPIO_DeInit
	.globl _GPIO_Init
	.globl _GPIO_Write
	.globl _GPIO_WriteHigh
	.globl _GPIO_WriteLow
	.globl _GPIO_WriteReverse
	.globl _GPIO_ReadOutputData
	.globl _GPIO_ReadInputData
	.globl _GPIO_ReadInputPin
	.globl _GPIO_ExternalPullUpConfig
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area DATA
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area INITIALIZED
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
; global & static initialisations
;--------------------------------------------------------
	.area HOME
	.area GSINIT
	.area GSFINAL
	.area GSINIT
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area HOME
	.area HOME
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE
	G$GPIO_DeInit$0$0 ==.
	C$stm8s_gpio.c$53$0_0$358 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 53: void GPIO_DeInit(GPIO_TypeDef* GPIOx)
; genLabel
;	-----------------------------------------
;	 function GPIO_DeInit
;	-----------------------------------------
;	Register assignment might be sub-optimal.
;	Stack space usage: 0 bytes.
_GPIO_DeInit:
; genReceive
	exgw	x, y
	C$stm8s_gpio.c$55$1_0$358 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 55: GPIOx->ODR = GPIO_ODR_RESET_VALUE; /* Reset Output Data Register */
; genPointerSet
	clr	(y)
	C$stm8s_gpio.c$56$1_0$358 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 56: GPIOx->DDR = GPIO_DDR_RESET_VALUE; /* Reset Data Direction Register */
; genPlus
	ldw	x, y
	incw	x
	incw	x
; genPointerSet
	clr	(x)
	C$stm8s_gpio.c$57$1_0$358 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 57: GPIOx->CR1 = GPIO_CR1_RESET_VALUE; /* Reset Control Register 1 */
; genPlus
	ldw	x, y
	addw	x, #0x0003
; genPointerSet
	clr	(x)
	C$stm8s_gpio.c$58$1_0$358 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 58: GPIOx->CR2 = GPIO_CR2_RESET_VALUE; /* Reset Control Register 2 */
; genPlus
	ldw	x, y
	addw	x, #0x0004
; genPointerSet
	clr	(x)
; genLabel
00101$:
	C$stm8s_gpio.c$59$1_0$358 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 59: }
; genEndFunction
	C$stm8s_gpio.c$59$1_0$358 ==.
	XG$GPIO_DeInit$0$0 ==.
	ret
	G$GPIO_Init$0$0 ==.
	C$stm8s_gpio.c$71$1_0$360 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 71: void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin, GPIO_Mode_TypeDef GPIO_Mode)
; genLabel
;	-----------------------------------------
;	 function GPIO_Init
;	-----------------------------------------
;	Register assignment might be sub-optimal.
;	Stack space usage: 6 bytes.
_GPIO_Init:
	sub	sp, #6
; genReceive
	ldw	(0x05, sp), x
; genReceive
	ld	(0x04, sp), a
	C$stm8s_gpio.c$77$1_0$360 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 77: assert_param(IS_GPIO_MODE_OK(GPIO_Mode));
; genIfx
	tnz	(0x09, sp)
	jrne	00271$
	jp	00116$
00271$:
; genCmpEQorNE
	ld	a, (0x09, sp)
	cp	a, #0x40
	jrne	00273$
	jp	00116$
00273$:
; skipping generated iCode
; genCmpEQorNE
	ld	a, (0x09, sp)
	cp	a, #0x20
	jrne	00276$
	jp	00116$
00276$:
; skipping generated iCode
; genCmpEQorNE
	ld	a, (0x09, sp)
	cp	a, #0x60
	jrne	00279$
	jp	00116$
00279$:
; skipping generated iCode
; genCmpEQorNE
	ld	a, (0x09, sp)
	cp	a, #0xa0
	jrne	00282$
	jp	00116$
00282$:
; skipping generated iCode
; genCmpEQorNE
	ld	a, (0x09, sp)
	cp	a, #0xe0
	jrne	00285$
	jp	00116$
00285$:
; skipping generated iCode
; genCmpEQorNE
	ld	a, (0x09, sp)
	cp	a, #0x80
	jrne	00288$
	jp	00116$
00288$:
; skipping generated iCode
; genCmpEQorNE
	ld	a, (0x09, sp)
	cp	a, #0xc0
	jrne	00291$
	jp	00116$
00291$:
; skipping generated iCode
; genCmpEQorNE
	ld	a, (0x09, sp)
	cp	a, #0xb0
	jrne	00294$
	jp	00116$
00294$:
; skipping generated iCode
; genCmpEQorNE
	ld	a, (0x09, sp)
	cp	a, #0xf0
	jrne	00297$
	jp	00116$
00297$:
; skipping generated iCode
; genCmpEQorNE
	ld	a, (0x09, sp)
	cp	a, #0x90
	jrne	00300$
	jp	00116$
00300$:
; skipping generated iCode
; genCmpEQorNE
	ld	a, (0x09, sp)
	cp	a, #0xd0
	jrne	00303$
	jp	00116$
00303$:
; skipping generated iCode
; skipping iCode since result will be rematerialized
; skipping iCode since result will be rematerialized
; genIPush
	push	#0x4d
	clrw	x
	pushw	x
	push	#0x00
; genSend
	ldw	x, #(___str_0+0)
; genCall
	call	_assert_failed
; genLabel
00116$:
	C$stm8s_gpio.c$78$1_0$360 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 78: assert_param(IS_GPIO_PIN_OK(GPIO_Pin));
; genIfx
	tnz	(0x04, sp)
	jreq	00305$
	jp	00151$
00305$:
; skipping iCode since result will be rematerialized
; skipping iCode since result will be rematerialized
; genIPush
	push	#0x4e
	clrw	x
	pushw	x
	push	#0x00
; genSend
	ldw	x, #(___str_0+0)
; genCall
	call	_assert_failed
; genLabel
00151$:
	C$stm8s_gpio.c$81$1_0$360 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 81: GPIOx->CR2 &= (uint8_t)(~(GPIO_Pin));
; genPlus
	ldw	x, (0x05, sp)
	addw	x, #0x0004
	ldw	(0x01, sp), x
; genPointerGet
	ldw	x, (0x01, sp)
	ld	a, (x)
; genCpl
	push	a
	ld	a, (0x05, sp)
	cpl	a
	ld	(0x04, sp), a
	pop	a
; genAnd
	and	a, (0x03, sp)
; genPointerSet
	ldw	x, (0x01, sp)
	ld	(x), a
	C$stm8s_gpio.c$98$1_0$360 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 98: GPIOx->DDR |= (uint8_t)GPIO_Pin;
; genPlus
	ldw	x, (0x05, sp)
	incw	x
	incw	x
	C$stm8s_gpio.c$87$1_0$360 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 87: if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x80) != (uint8_t)0x00) /* Output mode */
; genAnd
	tnz	(0x09, sp)
	jrmi	00306$
	jp	00105$
00306$:
; skipping generated iCode
	C$stm8s_gpio.c$91$1_0$360 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 91: GPIOx->ODR |= (uint8_t)GPIO_Pin;
; genPointerGet
	ldw	y, (0x05, sp)
	ld	a, (y)
	C$stm8s_gpio.c$89$2_0$361 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 89: if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x10) != (uint8_t)0x00) /* High level */
; genAnd
	push	a
	ld	a, (0x0a, sp)
	bcp	a, #0x10
	pop	a
	jrne	00307$
	jp	00102$
00307$:
; skipping generated iCode
	C$stm8s_gpio.c$91$3_0$362 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 91: GPIOx->ODR |= (uint8_t)GPIO_Pin;
; genOr
	or	a, (0x04, sp)
; genPointerSet
	ldw	y, (0x05, sp)
	ld	(y), a
; genGoto
	jp	00103$
; genLabel
00102$:
	C$stm8s_gpio.c$95$3_0$363 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 95: GPIOx->ODR &= (uint8_t)(~(GPIO_Pin));
; genAnd
	and	a, (0x03, sp)
; genPointerSet
	ldw	y, (0x05, sp)
	ld	(y), a
; genLabel
00103$:
	C$stm8s_gpio.c$98$2_0$361 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 98: GPIOx->DDR |= (uint8_t)GPIO_Pin;
; genPointerGet
	ld	a, (x)
; genOr
	or	a, (0x04, sp)
; genPointerSet
	ld	(x), a
; genGoto
	jp	00106$
; genLabel
00105$:
	C$stm8s_gpio.c$103$2_0$364 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 103: GPIOx->DDR &= (uint8_t)(~(GPIO_Pin));
; genPointerGet
	ld	a, (x)
; genAnd
	and	a, (0x03, sp)
; genPointerSet
	ld	(x), a
; genLabel
00106$:
	C$stm8s_gpio.c$112$1_0$360 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 112: GPIOx->CR1 |= (uint8_t)GPIO_Pin;
; genPlus
	ldw	x, (0x05, sp)
	addw	x, #0x0003
; genPointerGet
	ld	a, (x)
	C$stm8s_gpio.c$110$1_0$360 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 110: if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x40) != (uint8_t)0x00) /* Pull-Up or Push-Pull */
; genAnd
	push	a
	ld	a, (0x0a, sp)
	bcp	a, #0x40
	pop	a
	jrne	00308$
	jp	00108$
00308$:
; skipping generated iCode
	C$stm8s_gpio.c$112$2_0$365 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 112: GPIOx->CR1 |= (uint8_t)GPIO_Pin;
; genOr
	or	a, (0x04, sp)
; genPointerSet
	ld	(x), a
; genGoto
	jp	00109$
; genLabel
00108$:
	C$stm8s_gpio.c$116$2_0$366 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 116: GPIOx->CR1 &= (uint8_t)(~(GPIO_Pin));
; genAnd
	and	a, (0x03, sp)
; genPointerSet
	ld	(x), a
; genLabel
00109$:
	C$stm8s_gpio.c$81$1_0$360 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 81: GPIOx->CR2 &= (uint8_t)(~(GPIO_Pin));
; genPointerGet
	ldw	x, (0x01, sp)
	ld	a, (x)
	C$stm8s_gpio.c$123$1_0$360 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 123: if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x20) != (uint8_t)0x00) /* Interrupt or Slow slope */
; genAnd
	push	a
	ld	a, (0x0a, sp)
	bcp	a, #0x20
	pop	a
	jrne	00309$
	jp	00111$
00309$:
; skipping generated iCode
	C$stm8s_gpio.c$125$2_0$367 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 125: GPIOx->CR2 |= (uint8_t)GPIO_Pin;
; genOr
	or	a, (0x04, sp)
; genPointerSet
	ldw	x, (0x01, sp)
	ld	(x), a
; genGoto
	jp	00113$
; genLabel
00111$:
	C$stm8s_gpio.c$129$2_0$368 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 129: GPIOx->CR2 &= (uint8_t)(~(GPIO_Pin));
; genAnd
	and	a, (0x03, sp)
; genPointerSet
	ldw	x, (0x01, sp)
	ld	(x), a
; genLabel
00113$:
	C$stm8s_gpio.c$131$1_0$360 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 131: }
; genEndFunction
	addw	sp, #6
	popw	x
	pop	a
	jp	(x)
	G$GPIO_Write$0$0 ==.
	C$stm8s_gpio.c$141$1_0$370 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 141: void GPIO_Write(GPIO_TypeDef* GPIOx, uint8_t PortVal)
; genLabel
;	-----------------------------------------
;	 function GPIO_Write
;	-----------------------------------------
;	Register assignment is optimal.
;	Stack space usage: 0 bytes.
_GPIO_Write:
; genReceive
; genReceive
	C$stm8s_gpio.c$143$1_0$370 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 143: GPIOx->ODR = PortVal;
; genPointerSet
	ld	(x), a
; genLabel
00101$:
	C$stm8s_gpio.c$144$1_0$370 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 144: }
; genEndFunction
	C$stm8s_gpio.c$144$1_0$370 ==.
	XG$GPIO_Write$0$0 ==.
	ret
	G$GPIO_WriteHigh$0$0 ==.
	C$stm8s_gpio.c$154$1_0$372 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 154: void GPIO_WriteHigh(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
; genLabel
;	-----------------------------------------
;	 function GPIO_WriteHigh
;	-----------------------------------------
;	Register assignment is optimal.
;	Stack space usage: 1 bytes.
_GPIO_WriteHigh:
	push	a
; genReceive
; genReceive
	ld	(0x01, sp), a
	C$stm8s_gpio.c$156$1_0$372 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 156: GPIOx->ODR |= (uint8_t)PortPins;
; genPointerGet
	ld	a, (x)
; genOr
	or	a, (0x01, sp)
; genPointerSet
	ld	(x), a
; genLabel
00101$:
	C$stm8s_gpio.c$157$1_0$372 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 157: }
; genEndFunction
	pop	a
	C$stm8s_gpio.c$157$1_0$372 ==.
	XG$GPIO_WriteHigh$0$0 ==.
	ret
	G$GPIO_WriteLow$0$0 ==.
	C$stm8s_gpio.c$167$1_0$374 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 167: void GPIO_WriteLow(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
; genLabel
;	-----------------------------------------
;	 function GPIO_WriteLow
;	-----------------------------------------
;	Register assignment might be sub-optimal.
;	Stack space usage: 1 bytes.
_GPIO_WriteLow:
	push	a
; genReceive
; genReceive
	C$stm8s_gpio.c$169$1_0$374 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 169: GPIOx->ODR &= (uint8_t)(~PortPins);
; genPointerGet
	push	a
	ld	a, (x)
	ld	(0x02, sp), a
	pop	a
; genCpl
	cpl	a
; genAnd
	and	a, (0x01, sp)
; genPointerSet
	ld	(x), a
; genLabel
00101$:
	C$stm8s_gpio.c$170$1_0$374 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 170: }
; genEndFunction
	pop	a
	C$stm8s_gpio.c$170$1_0$374 ==.
	XG$GPIO_WriteLow$0$0 ==.
	ret
	G$GPIO_WriteReverse$0$0 ==.
	C$stm8s_gpio.c$180$1_0$376 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 180: void GPIO_WriteReverse(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
; genLabel
;	-----------------------------------------
;	 function GPIO_WriteReverse
;	-----------------------------------------
;	Register assignment is optimal.
;	Stack space usage: 1 bytes.
_GPIO_WriteReverse:
	push	a
; genReceive
; genReceive
	ld	(0x01, sp), a
	C$stm8s_gpio.c$182$1_0$376 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 182: GPIOx->ODR ^= (uint8_t)PortPins;
; genPointerGet
	ld	a, (x)
; genXor
	xor	a, (0x01, sp)
; genPointerSet
	ld	(x), a
; genLabel
00101$:
	C$stm8s_gpio.c$183$1_0$376 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 183: }
; genEndFunction
	pop	a
	C$stm8s_gpio.c$183$1_0$376 ==.
	XG$GPIO_WriteReverse$0$0 ==.
	ret
	G$GPIO_ReadOutputData$0$0 ==.
	C$stm8s_gpio.c$191$1_0$378 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 191: uint8_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx)
; genLabel
;	-----------------------------------------
;	 function GPIO_ReadOutputData
;	-----------------------------------------
;	Register assignment is optimal.
;	Stack space usage: 0 bytes.
_GPIO_ReadOutputData:
; genReceive
	C$stm8s_gpio.c$193$1_0$378 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 193: return ((uint8_t)GPIOx->ODR);
; genPointerGet
	ld	a, (x)
; genReturn
; genLabel
00101$:
	C$stm8s_gpio.c$194$1_0$378 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 194: }
; genEndFunction
	C$stm8s_gpio.c$194$1_0$378 ==.
	XG$GPIO_ReadOutputData$0$0 ==.
	ret
	G$GPIO_ReadInputData$0$0 ==.
	C$stm8s_gpio.c$202$1_0$380 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 202: uint8_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx)
; genLabel
;	-----------------------------------------
;	 function GPIO_ReadInputData
;	-----------------------------------------
;	Register assignment might be sub-optimal.
;	Stack space usage: 0 bytes.
_GPIO_ReadInputData:
; genReceive
	C$stm8s_gpio.c$204$1_0$380 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 204: return ((uint8_t)GPIOx->IDR);
; genAssign
; genPointerGet
	ld	a, (0x1, x)
; genReturn
; genLabel
00101$:
	C$stm8s_gpio.c$205$1_0$380 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 205: }
; genEndFunction
	C$stm8s_gpio.c$205$1_0$380 ==.
	XG$GPIO_ReadInputData$0$0 ==.
	ret
	G$GPIO_ReadInputPin$0$0 ==.
	C$stm8s_gpio.c$213$1_0$382 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 213: BitStatus GPIO_ReadInputPin(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin)
; genLabel
;	-----------------------------------------
;	 function GPIO_ReadInputPin
;	-----------------------------------------
;	Register assignment might be sub-optimal.
;	Stack space usage: 1 bytes.
_GPIO_ReadInputPin:
	push	a
; genReceive
; genReceive
	ld	(0x01, sp), a
	C$stm8s_gpio.c$215$1_0$382 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 215: return ((BitStatus)(GPIOx->IDR & (uint8_t)GPIO_Pin));
; genAssign
; genPointerGet
	ld	a, (0x1, x)
; genAnd
	and	a, (0x01, sp)
; genCast
	neg	a
	clr	a
	rlc	a
; genReturn
; genLabel
00101$:
	C$stm8s_gpio.c$216$1_0$382 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 216: }
; genEndFunction
	addw	sp, #1
	C$stm8s_gpio.c$216$1_0$382 ==.
	XG$GPIO_ReadInputPin$0$0 ==.
	ret
	G$GPIO_ExternalPullUpConfig$0$0 ==.
	C$stm8s_gpio.c$225$1_0$384 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 225: void GPIO_ExternalPullUpConfig(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin, FunctionalState NewState)
; genLabel
;	-----------------------------------------
;	 function GPIO_ExternalPullUpConfig
;	-----------------------------------------
;	Register assignment might be sub-optimal.
;	Stack space usage: 3 bytes.
_GPIO_ExternalPullUpConfig:
	sub	sp, #3
; genReceive
	ldw	(0x02, sp), x
; genReceive
	C$stm8s_gpio.c$228$1_0$384 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 228: assert_param(IS_GPIO_PIN_OK(GPIO_Pin));
; genIfx
	tnz	a
	jreq	00141$
	jp	00107$
00141$:
; skipping iCode since result will be rematerialized
; skipping iCode since result will be rematerialized
; genIPush
	push	a
	push	#0xe4
	clrw	x
	pushw	x
	push	#0x00
; genSend
	ldw	x, #(___str_0+0)
; genCall
	call	_assert_failed
	pop	a
; genLabel
00107$:
	C$stm8s_gpio.c$229$1_0$384 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 229: assert_param(IS_FUNCTIONALSTATE_OK(NewState));
; genIfx
	tnz	(0x06, sp)
	jrne	00142$
	jp	00109$
00142$:
; genIfx
	tnz	(0x06, sp)
	jreq	00143$
	jp	00109$
00143$:
; skipping iCode since result will be rematerialized
; skipping iCode since result will be rematerialized
; genIPush
	push	a
	push	#0xe5
	clrw	x
	pushw	x
	push	#0x00
; genSend
	ldw	x, #(___str_0+0)
; genCall
	call	_assert_failed
	pop	a
; genLabel
00109$:
	C$stm8s_gpio.c$233$1_0$384 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 233: GPIOx->CR1 |= (uint8_t)GPIO_Pin;
; genPlus
	ldw	x, (0x02, sp)
	addw	x, #0x0003
; genPointerGet
	push	a
	ld	a, (x)
	ld	(0x02, sp), a
	pop	a
	C$stm8s_gpio.c$231$1_0$384 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 231: if (NewState != DISABLE) /* External Pull-Up Set*/
; genIfx
	tnz	(0x06, sp)
	jrne	00144$
	jp	00102$
00144$:
	C$stm8s_gpio.c$233$2_0$385 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 233: GPIOx->CR1 |= (uint8_t)GPIO_Pin;
; genOr
	or	a, (0x01, sp)
; genPointerSet
	ld	(x), a
; genGoto
	jp	00104$
; genLabel
00102$:
	C$stm8s_gpio.c$236$2_0$386 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 236: GPIOx->CR1 &= (uint8_t)(~(GPIO_Pin));
; genCpl
	cpl	a
; genAnd
	and	a, (0x01, sp)
; genPointerSet
	ld	(x), a
; genLabel
00104$:
	C$stm8s_gpio.c$238$1_0$384 ==.
;	STM8S_StdPeriph_Driver/src/stm8s_gpio.c: 238: }
; genEndFunction
	addw	sp, #3
	popw	x
	pop	a
	jp	(x)
	.area CODE
	.area CONST
Fstm8s_gpio$__str_0$0_0$0 == .
	.area CONST
___str_0:
	.ascii "STM8S_StdPeriph_Driver/src/stm8s_gpio.c"
	.db 0x00
	.area CODE
	.area INITIALIZER
	.area CABS (ABS)
