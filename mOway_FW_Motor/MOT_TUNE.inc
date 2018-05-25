;***********************************************************
;
;		DESCRIPTION:	Functions to get the KRC values
;									of mOway encoders and save them in EEPROM
;
;		DATE:	03/05/2013
;
;***********************************************************

;*************************************************
;		FUNCTION:	READ_TUN
;		DESCRIPTION: Read KRC levels from EEPROM
;*************************************************
READ_TUN:
	
	movlw	EE_KRC_SUP_L			; Read left KRC level
	movwf	EE_USER_ADD			
	call EE_READ
	movfw	EE_USER_DATA
	movwf LIMIT_HIGH_L
	movlw	EE_KRC_INF_L			; Read left KRC level
	movwf	EE_USER_ADD			
	call EE_READ
	movfw	EE_USER_DATA
	movwf LIMIT_LOW_L
	
	movlw	EE_KRC_SUP_R			; Read read KRC level
	movwf	EE_USER_ADD			
	call EE_READ
	movfw	EE_USER_DATA
	movwf LIMIT_HIGH_R
	movlw	EE_KRC_INF_R			; Read read KRC level
	movwf	EE_USER_ADD			
	call EE_READ
	movfw	EE_USER_DATA
	movwf LIMIT_LOW_R
			
	return
	
;*************************************************
;		FUNCTION:	MOVE_TUN_GO_LEFT
;		DESCRIPTION: Move left wheel in 7.5º
;*************************************************
MOVE_TUN_GO_LEFT:

	;The encoders are activated
	banksel	PORTA

	bcf		PORTA,MOTOR_L_A
	bsf		PORTA,MOTOR_L_B
	movlw	.5
	movwf	TUN_DELAY2
	movlw	.255
	movwf	TUN_DELAY1
TUN_LOOP1_GO_L:
	nop
	decfsz	TUN_DELAY1,F
	goto	TUN_LOOP1_GO_L
	decfsz	TUN_DELAY2,F
	goto	TUN_LOOP1_GO_L
	
	bcf		PORTA,MOTOR_L_B
	movlw	.5
	movwf	TUN_DELAY2
	movlw	.255
	movwf	TUN_DELAY1
TUN_LOOP2_GO_L:
	nop
	decfsz	TUN_DELAY1,F
	goto	TUN_LOOP2_GO_L
	decfsz	TUN_DELAY2,F
	goto	TUN_LOOP2_GO_L

	return
	
;*************************************************
;		FUNCTION:	MOVE_TUN_BACK_LEFT
;		DESCRIPTION: Move left wheel in 7.5º
;*************************************************
MOVE_TUN_BACK_LEFT:

	;The encoders are activated
	banksel	PORTA

	bsf		PORTA,MOTOR_L_A
	bcf		PORTA,MOTOR_L_B
	movlw	.5
	movwf	TUN_DELAY2
	movlw	.255
	movwf	TUN_DELAY1
TUN_LOOP1_BACK_L:
	nop
	decfsz	TUN_DELAY1,F
	goto	TUN_LOOP1_BACK_L
	decfsz	TUN_DELAY2,F
	goto	TUN_LOOP1_BACK_L
	
	bcf		PORTA,MOTOR_L_A
	movlw	.5
	movwf	TUN_DELAY2
	movlw	.255
	movwf	TUN_DELAY1
TUN_LOOP2_BACK_L:
	nop
	decfsz	TUN_DELAY1,F
	goto	TUN_LOOP2_BACK_L
	decfsz	TUN_DELAY2,F
	goto	TUN_LOOP2_BACK_L

	return
	
;*************************************************
;		FUNCTION:	MOVE_TUN_GO_RIGHT
;		DESCRIPTION: Move right wheel in 7.5º
;*************************************************
MOVE_TUN_GO_RIGHT:

	;The encoders are activated
	banksel	PORTA

	bcf		PORTC,MOTOR_R_A
	bsf		PORTC,MOTOR_R_B
	movlw	.5
	movwf	TUN_DELAY2
	movlw	.255
	movwf	TUN_DELAY1
TUN_LOOP1_GO_R:
	nop
	decfsz	TUN_DELAY1,F
	goto	TUN_LOOP1_GO_R
	decfsz	TUN_DELAY2,F
	goto	TUN_LOOP1_GO_R
	
	bcf		PORTC,MOTOR_R_B
	movlw	.5
	movwf	TUN_DELAY2
	movlw	.255
	movwf	TUN_DELAY1
TUN_LOOP2_GO_R:
	nop
	decfsz	TUN_DELAY1,F
	goto	TUN_LOOP2_GO_R
	decfsz	TUN_DELAY2,F
	goto	TUN_LOOP2_GO_R

	return
	

;*************************************************
;		FUNCTION:	MOVE_TUN_BACK_RIGHT
;		DESCRIPTION: Move right wheel in 7.5º
;*************************************************
MOVE_TUN_BACK_RIGHT:

	;The encoders are activated
	banksel	PORTA

	bsf		PORTC,MOTOR_R_A
	bcf		PORTC,MOTOR_R_B
	movlw	.5
	movwf	TUN_DELAY2
	movlw	.255
	movwf	TUN_DELAY1
TUN_LOOP1_BACK_R:
	nop
	decfsz	TUN_DELAY1,F
	goto	TUN_LOOP1_BACK_R
	decfsz	TUN_DELAY2,F
	goto	TUN_LOOP1_BACK_R
	
	bcf		PORTC,MOTOR_R_A
	movlw	.5
	movwf	TUN_DELAY2
	movlw	.255
	movwf	TUN_DELAY1
TUN_LOOP2_BACK_R:
	nop
	decfsz	TUN_DELAY1,F
	goto	TUN_LOOP2_BACK_R
	decfsz	TUN_DELAY2,F
	goto	TUN_LOOP2_BACK_R

	return

	
;*************************************************
;		FUNCTION:	CHECK_KRC_MAX
;		DESCRIPTION: Get KRC maximum level
;*************************************************
CHECK_KRC_MAX:
	subwf	KRC_MAX, W			; W > f -> C = 0
	btfsc	STATUS, C
	goto	POSITIVE
	movfw	EE_USER_DATA		; Negative
	movwf	KRC_MAX
POSITIVE:
	return
	
;*************************************************
;		FUNCTION:	CHECK_KRC_MIN
;		DESCRIPTION: Get KRC minimum level
;*************************************************
CHECK_KRC_MIN:
	subwf	KRC_MIN, W			; W > f -> C = 0
	btfss	STATUS, C
	goto	NEGATIVE
	movfw	EE_USER_DATA		; Positive
	movwf	KRC_MIN
NEGATIVE:
	return

	
;*********************************************
;		Autotune command
;*********************************************
TUNE_COM:	
; Generate a low level pulse of 10 ms 
; for sending "Firmware OK" to PC application
	bcf		PORTC,FINISHED
	movlw	.50					; 50 x 100 x 2us = 10ms
	movwf	TUN_DELAY2
TUN_LOOP1_APP:
	movlw	.100
	movwf	TUN_DELAY1
TUN_LOOP2_APP:
	nop
	decfsz	TUN_DELAY1,F
	goto	TUN_LOOP2_APP
	decfsz	TUN_DELAY2,F
	goto	TUN_LOOP1_APP
	bsf		PORTC,FINISHED
	
	; Previous movement (KRC level varies)
	movlw	.255
	movwf	TEMP_VALUE
TUNE_PREV1_GO_L:
	call	MOVE_TUN_GO_LEFT	; Move encoder	
	bcf	STATUS,Z
	decfsz	TEMP_VALUE,F
	goto TUNE_PREV1_GO_L
	
	movlw	.100
	movwf	TEMP_VALUE
TUNE_PREV2_GO_L:
	call	MOVE_TUN_GO_LEFT	; Move encoder	
	bcf	STATUS,Z
	decfsz	TEMP_VALUE,F
	goto TUNE_PREV2_GO_L
		
	movlw	.0					; Initialize KRC variables
	movwf	KRC_MAX
	movlw	.255
	movwf	KRC_MIN	
	
	;****************************************************
	;		LEFT KRC GO
	;****************************************************	
	movlw	.48					; Save 48 values of KRC (360º/48 -> 7.5º)
	movwf	TEMP_VALUE
TUNE_LOOP_GO_L:
	call	MOVE_TUN_GO_LEFT	; Move encoder
	call READ_ANALOG_LEFT		; Read KRC
	movfw	ANALOG_LEFT	
	movwf	EE_USER_DATA		; Save KRC data on EEPROM	
	call	CHECK_KRC_MAX
	movfw	EE_USER_DATA
	call	CHECK_KRC_MIN	
		
	bcf	STATUS,Z
	decfsz	TEMP_VALUE,F		; Decrease EEPROM address
	goto TUNE_LOOP_GO_L
	
	call	MOVE_TUN_GO_LEFT	; Last KRC data (add 0x00)
	call READ_ANALOG_LEFT
	movfw	ANALOG_LEFT	
	movwf	EE_USER_DATA	
	call	CHECK_KRC_MAX
	movfw	EE_USER_DATA
	call	CHECK_KRC_MIN		
	
	movfw	KRC_MAX				; Save max and min values
	movwf	EE_USER_DATA
	movlw	EE_KRC_MAX_GO_L
	movwf	EE_USER_ADD			
	call EE_WRITE
	
		
	movfw	KRC_MAX
	bcf		STATUS, C
	addwf	KRC_MIN, F
	rrf		KRC_MIN, W		
	movwf	EE_USER_DATA
	movlw	EE_KRC_GO_L
	movwf	EE_USER_ADD			
	call EE_WRITE
;*************************************************************************

	
	;*************************************************************************
	; Previous movement (KRC level varies)
	movlw	.255
	movwf	TEMP_VALUE
TUNE_PREV1_BACK_L:
	call	MOVE_TUN_BACK_LEFT			; Move encoder	
	bcf	STATUS,Z
	decfsz	TEMP_VALUE,F
	goto TUNE_PREV1_BACK_L
	
	movlw	.100
	movwf	TEMP_VALUE
TUNE_PREV2_BACK_L:
	call	MOVE_TUN_BACK_LEFT			; Move encoder	
	bcf	STATUS,Z
	decfsz	TEMP_VALUE,F
	goto TUNE_PREV2_BACK_L
		
	movlw	.0					; Initialize KRC variables
	movwf	KRC_MAX
	movlw	.255
	movwf	KRC_MIN	
	
	;****************************************************
	;		LEFT KRC BACK
	;****************************************************	
	movlw	.48					; Save 48 values of KRC (360º/48 -> 7.5º)
	movwf	TEMP_VALUE
TUNE_LOOP_BACK_L:
	call	MOVE_TUN_BACK_LEFT	; Move encoder
	call READ_ANALOG_LEFT		; Read KRC
	movfw	ANALOG_LEFT	
	movwf	EE_USER_DATA		; Save KRC data on EEPROM	
	call	CHECK_KRC_MAX
	movfw	EE_USER_DATA
	call	CHECK_KRC_MIN	
		
	bcf	STATUS,Z
	decfsz	TEMP_VALUE,F		; Decrease EEPROM address
	goto TUNE_LOOP_BACK_L
	
	call	MOVE_TUN_BACK_LEFT	; Last KRC data (add 0x00)
	call READ_ANALOG_LEFT
	movfw	ANALOG_LEFT	
	movwf	EE_USER_DATA	
	call	CHECK_KRC_MAX
	movfw	EE_USER_DATA
	call	CHECK_KRC_MIN		
	
	
	movfw	KRC_MAX
	bcf		STATUS, C
	addwf	KRC_MIN, F
	rrf		KRC_MIN, W		
	movwf	EE_USER_DATA	
	movlw	EE_KRC_BACK_L
	movwf	EE_USER_ADD			
	call EE_WRITE
	
	
	;****************************************************
	;		RIGHT KRC GO
	;****************************************************	
	;*************************************************************************
	; Previous movement (KRC level varies)
	movlw	.255
	movwf	TEMP_VALUE
TUNE_PREV1_GO_R:
	call	MOVE_TUN_GO_RIGHT	; Move encoder	
	bcf	STATUS,Z
	decfsz	TEMP_VALUE,F
	goto TUNE_PREV1_GO_R
	
	movlw	.100
	movwf	TEMP_VALUE
TUNE_PREV2_GO_R:
	call	MOVE_TUN_GO_RIGHT	; Move encoder	
	bcf	STATUS,Z
	decfsz	TEMP_VALUE,F
	goto TUNE_PREV2_GO_R
	
	movlw	.0					; Initialize KRC variables
	movwf	KRC_MAX
	movlw	.255
	movwf	KRC_MIN
	movlw	.48					; Save 48 values of KRC (360º/48 -> 7.5º)
	movwf	TEMP_VALUE
	
TUNE_LOOP_GO_R:
	call	MOVE_TUN_GO_RIGHT	; Move encoder
	call READ_ANALOG_RIGHT		; Read KRC
	movfw	ANALOG_RIGHT	
	movwf	EE_USER_DATA		; Save KRC data on EEPROM	
	call	CHECK_KRC_MAX
	movfw	EE_USER_DATA
	call	CHECK_KRC_MIN	
		
	bcf	STATUS,Z
	decfsz	TEMP_VALUE,F		; Decrease EEPROM address
	goto TUNE_LOOP_GO_R
	
	call	MOVE_TUN_GO_RIGHT	; Last KRC data (add 0x00)
	call READ_ANALOG_RIGHT
	movfw	ANALOG_RIGHT	
	movwf	EE_USER_DATA	
	call	CHECK_KRC_MAX
	movfw	EE_USER_DATA
	call	CHECK_KRC_MIN		
	
	movfw	KRC_MAX				; Save max and min values
	movwf	EE_USER_DATA
	movlw	EE_KRC_MAX_GO_R
	movwf	EE_USER_ADD			
	call EE_WRITE
	
		
	movfw	KRC_MAX				; Calculate the average
	bcf		STATUS, C
	addwf	KRC_MIN, F
	rrf		KRC_MIN, W		
	movwf	EE_USER_DATA
	movlw	EE_KRC_GO_R
	movwf	EE_USER_ADD			
	call EE_WRITE	
	
	;****************************************************
	;		RIGHT KRC BACK
	;****************************************************	
	;*************************************************************************
	; Previous movement (KRC level varies)
	movlw	.255
	movwf	TEMP_VALUE
TUNE_PREV1_BACK_R:
	call	MOVE_TUN_BACK_RIGHT		; Move encoder	
	bcf	STATUS,Z
	decfsz	TEMP_VALUE,F
	goto TUNE_PREV1_BACK_R
	
	movlw	.100
	movwf	TEMP_VALUE
TUNE_PREV2_BACK_R:
	call	MOVE_TUN_BACK_RIGHT		; Move encoder	
	bcf	STATUS,Z
	decfsz	TEMP_VALUE,F
	goto TUNE_PREV2_BACK_R
	
	movlw	.0					; Initialize KRC variables
	movwf	KRC_MAX
	movlw	.255
	movwf	KRC_MIN
	movlw	.48					; Save 48 values of KRC (360º/48 -> 7.5º)
	movwf	TEMP_VALUE
	
TUNE_LOOP_BACK_R:
	call	MOVE_TUN_BACK_RIGHT		; Move encoder
	call 	READ_ANALOG_RIGHT		; Read KRC
	movfw	ANALOG_RIGHT	
	movwf	EE_USER_DATA			; Save KRC data on EEPROM	
	call	CHECK_KRC_MAX
	movfw	EE_USER_DATA
	call	CHECK_KRC_MIN	
		
	bcf	STATUS,Z
	decfsz	TEMP_VALUE,F		; Decrease EEPROM address
	goto TUNE_LOOP_BACK_R
	
	call	MOVE_TUN_BACK_RIGHT		; Last KRC data (add 0x00)
	call READ_ANALOG_RIGHT
	movfw	ANALOG_RIGHT	
	movwf	EE_USER_DATA	
	call	CHECK_KRC_MAX
	movfw	EE_USER_DATA
	call	CHECK_KRC_MIN		
	
	movfw	KRC_MAX				; Calculate the average
	bcf		STATUS, C
	addwf	KRC_MIN, F
	rrf		KRC_MIN, W		
	movwf	EE_USER_DATA
	movlw	EE_KRC_BACK_R
	movwf	EE_USER_ADD			
	call EE_WRITE
	
	
	;**********************************
	;		SAVE VALID DATA
	;**********************************
	movlw	EE_KRC_GO_L			; Read left go KRC level
	movwf	EE_USER_ADD			
	call EE_READ
	movfw	EE_USER_DATA
	movwf	TEMP_VALUE			
	
	movlw	EE_KRC_BACK_L		; Read left back KRC level
	movwf	EE_USER_ADD			
	call EE_READ
	movfw	EE_USER_DATA
		
	; Compare left "go" and "back" KRC values
	subwf	TEMP_VALUE, W		; W > f -> C = 0
	btfsc	STATUS, C
	goto	JUMP_L_1
	movlw	EE_KRC_BACK_L		; "Back" level is greater.
	movwf	EE_USER_ADD			; Read left back KRC level for saving it in EEPROM
	call 	EE_READ
	movfw	EE_USER_DATA
	goto	JUMP_L_2
JUMP_L_1:
	movfw	TEMP_VALUE			; "Go" level is greater
JUMP_L_2:
	movwf	EE_USER_DATA		; Write valid left KRC data	in EEPROM
	movlw	EE_KRC_L
	movwf	EE_USER_ADD			
	call 	EE_WRITE
		
	movlw	EE_KRC_GO_R			; Read right go KRC level
	movwf	EE_USER_ADD			
	call 	EE_READ
	movfw	EE_USER_DATA
	movwf	TEMP_VALUE			
	
	movlw	EE_KRC_BACK_R		; Read right back KRC level
	movwf	EE_USER_ADD			
	call 	EE_READ
	movfw	EE_USER_DATA
		
	; Compare right "go" and "back" KRC values
	subwf	TEMP_VALUE, W		; W > f -> C = 0
	btfsc	STATUS, C
	goto	JUMP_R_1
	movlw	EE_KRC_BACK_R		; "Back" level is greater.
	movwf	EE_USER_ADD			; Read left back KRC level for saving it in EEPROM
	call 	EE_READ
	movfw	EE_USER_DATA
	goto JUMP_R_2
JUMP_R_1:
	movfw	TEMP_VALUE			; "Go" level is greater
JUMP_R_2:
	movwf	EE_USER_DATA		; Write valid right KRC data in EEPROM
	movlw	EE_KRC_R
	movwf	EE_USER_ADD
	call 	EE_WRITE
	
	;****************************************************************
	;		Left KRC values
	;****************************************************************
	; Read the valid values and save into variables
	movlw	EE_KRC_L			; Save left KRC level
	movwf	EE_USER_ADD			
	call 	EE_READ
	movfw	EE_USER_DATA	
	;***DEBUG
	movwf LIMIT_L				; Save KRC average value
	movwf	TEMP_VALUE
	bcf	STATUS,C
	rrf	TEMP_VALUE				; Divide by 2 (for SUBWF)
	
	movlw	EE_KRC_MAX_GO_L		; Read left max KRC level
	movwf	EE_USER_ADD			
	call 	EE_READ
	movfw	EE_USER_DATA
	movwf	TEMP_VALUE2
	bcf	STATUS,C
	rrf	TEMP_VALUE2				; Divide by 2 (for SUBWF)
	
	movfw	TEMP_VALUE		
	subwf	TEMP_VALUE2,F		; TEMP_VALUE2 = (KRC_MAX - LIMIT_L) / 2
	movfw	LIMIT_L
	addwf	TEMP_VALUE2,W
	movwf	LIMIT_HIGH_L		; Left upper level limit	
	movwf	EE_USER_DATA		; Save in EEPROM
	movlw	EE_KRC_SUP_L
	movwf	EE_USER_ADD
	call 	EE_WRITE
	
	bcf	STATUS,C
	rrf	TEMP_VALUE2,F			; Difference divided by 4 for the lower limit	
	bcf	STATUS,C				; TEMP_VALUE2 = (KRC_MAX - LIMIT_L) / 8
	rrf	TEMP_VALUE2,F

	
	movfw	LIMIT_L				;***DEBUG	
	movwf	LIMIT_LOW_L
	movwf	EE_USER_DATA		; Save in EEPROM
	movlw	EE_KRC_INF_L
	movwf	EE_USER_ADD
	call 	EE_WRITE
	
	;****************************************************************
	;		Right KRC values
	;****************************************************************
	movlw	EE_KRC_R			; Save right KRC level
	movwf	EE_USER_ADD			
	call 	EE_READ
	movfw	EE_USER_DATA	
	
	;***DEBUG
	movwf LIMIT_R				; Save KRC average value
	movwf	TEMP_VALUE
	bcf	STATUS,C
	rrf	TEMP_VALUE				; Divide by 2 (for SUBWF)
	
	movlw	EE_KRC_MAX_GO_R		; Read left max KRC level
	movwf	EE_USER_ADD			
	call EE_READ
	movfw	EE_USER_DATA
	movwf	TEMP_VALUE2
	bcf	STATUS,C
	rrf	TEMP_VALUE2				; Divide by 2 (for SUBWF)
	
	movfw	TEMP_VALUE		
	subwf	TEMP_VALUE2,F		; TEMP_VALUE2 = (KRC_MAX - LIMIT_L) / 2
	movfw	LIMIT_R
	addwf	TEMP_VALUE2,W
	movwf	LIMIT_HIGH_R		; Left upper level limit	
	movwf	EE_USER_DATA		; Save in EEPROM
	movlw	EE_KRC_SUP_R
	movwf	EE_USER_ADD
	call EE_WRITE
	
	bcf	STATUS,C
	rrf	TEMP_VALUE2,F			; Difference divided by 4 for the lower limit	
	bcf	STATUS,C				; TEMP_VALUE2 = (KRC_MAX - LIMIT_L) / 8
	rrf	TEMP_VALUE2,F

	movfw	LIMIT_R				;***DEBUG
	movwf	LIMIT_LOW_R
	
	movwf	EE_USER_DATA		; Save in EEPROM
	movlw	EE_KRC_INF_R
	movwf	EE_USER_ADD
	call 	EE_WRITE
	
	;****************************************************
	;		Stop motors
	;****************************************************	
	banksel	INTCON

	bcf		INTCON,T0IE			;Interruption timer0 is disabled
	banksel	PIE1
	bcf		PIE1,TMR1IE			;Interruption timer1 is disabled
	
	bcf		STATUS,RP0
	bcf		STATUS,RP1

	;Clean PWM interrupts
	bcf		PIR1,TMR1IF
	bcf		INTCON,T0IF
	bsf		PORTA,MOTOR_L_A
	bsf		PORTA,MOTOR_L_B
	bsf		PORTC,MOTOR_R_A
	bsf		PORTC,MOTOR_R_B
	bcf		CONTROL_FLAGS,STOP_INIT
	bcf		CONTROL_FLAGS,MOVE_INIT
	bsf		PORTC,FINISHED
	
	return

		
;*******************************************************
;
;	    Functions to handle EEPROM data
;
;*******************************************************
EE_READ:
	; Set address to write
	banksel EE_USER_ADD
	movfw	EE_USER_ADD
	banksel EEADR
	movwf	EEADR
		
	BANKSEL EECON1
	BCF EECON1, EEPGD 		;Point to DATA memory
	BSF EECON1, RD 			;EE Read
	BANKSEL EEDAT
	MOVF EEDAT, W
	banksel EE_USER_DATA
	movwf	EE_USER_DATA	
	return
		
EE_WRITE:
	
	; Set data to write
	banksel EE_USER_DATA
	movf	EE_USER_DATA,W	
	banksel EEDATA
	movwf	EEDATA
	
	; Set address to write
	banksel EE_USER_ADD
	movfw	EE_USER_ADD
	banksel EEADR
	movwf	EEADR
	
	banksel EECON1
	bcf	 EECON1,EEPGD	 	; points to data memory
	bsf	 EECON1,WREN		; enable write
	bcf	 INTCON,GIE	 		; disable all ints
	movlw	0x55	 		;
	movwf	EECON2	 		; init write sequence
	movlw	0xAA	 		; using 2 instructions	
	movwf	EECON2	 		;
	bsf	 EECON1,WR	 		; write data
	bsf	 INTCON,GIE	 		; turn ints back on
	bcf	 STATUS,RP0	 		; select bank0
	bcf	 STATUS,RP1	 		;
wd01:
	btfss	PIR2,EEIF	 	; is int bit on ie WRTDONE
	goto	wd01	 		; if done continue	

	bcf	 PIR2,EEIF	 		; reset interupt
	banksel	EECON1
	bcf	 EECON1,WREN	 	; disables write
	bcf	 STATUS,RP1	 		; select bank1
	bcf	 STATUS,RP0	 		; select bank0
	
	return
	
	
