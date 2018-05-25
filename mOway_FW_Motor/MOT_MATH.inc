;//**********************/**/ CALCULATE_DIST_WHEEL /**/****************************//
;//*****/**/To calculate the distance to travel, multiply by 2.12/**/************//
;//******************************************************************************//
CALCULATE_DIST_WHEEL

;copy accumulator to temporary
	movf	BUF_I2C, w
	movwf	TEMP0


;shift accumulator right 2 times
	clrc
	rrf		BUF_I2C0, f
	clrc
	rrf		BUF_I2C0, f

;substract temporary from accumulator
	subwf	BUF_I2C0, f

;shift accumulator right 4 times
	swapf	BUF_I2C0, w
	andlw	0x0F
	movwf	BUF_I2C0
	movlw	0xF0
	skpc
	iorwf	BUF_I2C0, f

;add temporary to accumulator
	movf	TEMP0, w
	addwf	BUF_I2C0, f

;shift accumulator right 3 times
	clrc
	rrf		BUF_I2C0, f
	clrc
	rrf		BUF_I2C0, f
	clrc
	rrf		BUF_I2C0, f

;shift temporary left 1 times
	clrc
	rlf		TEMP0, f
	clrf	TEMP1
	rlf		TEMP1, f

;add temporary to accumulator
	clrf	BUF_I2C1
	movf	TEMP0, w
	addwf	BUF_I2C0, f
	movf	TEMP1, w
	skpnc
	incfsz	TEMP1, w
	addwf	BUF_I2C1, f


;If the speed is less than 80, one more step is added to 
;compensate for the inertia of high speeds
	movfw	SPEED_L
	sublw	SPEED_MIN
	btfsc	STATUS,Z
	goto	CALCULATE_DIST_WHEEL_00
	addlw	.174				;>80
	btfss	STATUS,C
	goto	CALCULATE_DIST_WHEEL_02
	decf	BUF_I2C0,W
	btfss	STATUS,Z
	movwf	BUF_I2C0
	decf	BUF_I2C0,W
	btfss	STATUS,Z
	movwf	BUF_I2C0
CALCULATE_DIST_WHEEL_02
	movfw	BUF_I2C0
	return
CALCULATE_DIST_WHEEL_00
	movfw	SPEED_R
	sublw	SPEED_MIN
	btfsc	STATUS,Z
	goto	CALCULATE_DIST_WHEEL_01
	addlw	.174				;>80
	btfss	STATUS,C
	goto	CALCULATE_DIST_WHEEL_01
	decf	BUF_I2C0,W
	btfss	STATUS,Z
	movwf	BUF_I2C0
	decf	BUF_I2C0,W
	btfss	STATUS,Z
	movwf	BUF_I2C0
CALCULATE_DIST_WHEEL_01
	movfw	BUF_I2C0
	return


;//**********************/**/ CALCULATE_DIST_CENTER /**/****************************//
;//*****/**/To calculate the distance to travel, multiply by 1.06/**/*********//
;//*******************************************************************************//
CALCULATE_DIST_CENTER
	
;If the speed is less than 80, one more step is added to 
;compensate for the inertia of high speeds
	movfw	SPEED_L
	sublw	SPEED_MIN
	addlw	.174				;>80
	btfss	STATUS,C
	incf	BUF_I2C,F
	
;copy accumulator to temporary
	movf	BUF_I2C, w
	movwf	TEMP0


;shift accumulator right 3 times
	clrc
	rrf	BUF_I2C0, f
	clrc
	rrf	BUF_I2C0, f
	clrc
	rrf	BUF_I2C0, f

;negate accumulator
	comf	BUF_I2C0, f
	incf	BUF_I2C0, f

;add temporary to accumulator
	addwf	BUF_I2C0, f

;shift accumulator right 1 times
	clrc
	rrf	BUF_I2C0, f

;add temporary to accumulator
	addwf	BUF_I2C0, f

;shift accumulator right 2 times
	rrf	BUF_I2C0, f
	clrc
	rrf	BUF_I2C0, f

;substract temporary from accumulator
	subwf	BUF_I2C0, f

;shift accumulator right 4 times
	swapf	BUF_I2C0, w
	andlw	0x0F
	movwf	BUF_I2C0
	movlw	0xF0
	skpc
	iorwf	BUF_I2C0, f

;add temporary to accumulator
	movf	TEMP0, w
	addwf	BUF_I2C0, f

;shift accumulator right 4 times
	swapf	BUF_I2C0, w
	andlw	0x0F
	movwf	BUF_I2C0

;add temporary to accumulator
	movf	TEMP0, w
	addwf	BUF_I2C0, f
	clrf	BUF_I2C1
	rlf	BUF_I2C1, f



;	;The minimum distance is limited to 1cm (problems with time for motor control and investment)
;	;It is checked if the distances are less than or equal 5.
	movfw	BUF_I2C0

	return


;//**********************/**/ CAL_ANGLE_WHEEL /**/  ****************************//
;//*******/**/To calculate the distance to travel, multiply by 0.4716	/**/*********//
;//******************************************************************************//
CAL_ANGLE_WHEEL

	btfsc	FLAGS_COM,STOP_ANG
	goto	HIGHER_Q_100_WHEEL

	movfw	DIST_AUX_R
	btfss	FLAGS_PWM,SPEED_0_L
	movfw	DIST_AUX_L

	movwf	BUF_I2C0
;copy accumulator to temporary
	movf	BUF_I2C0, w
	movwf	TEMP0


;shift accumulator right 1 times
	clrc
	rrf		BUF_I2C0, f

;add temporary to accumulator
	addwf	BUF_I2C0, f

;shift accumulator right 4 times
	swapf	BUF_I2C0, w
	andlw	0x0F
	movwf	BUF_I2C0
	skpnc
	bsf		BUF_I2C0, 4

;substract temporary from accumulator
	movf	TEMP0, w
	subwf	BUF_I2C0, f

;shift accumulator right 4 times
	swapf	BUF_I2C0, w
	andlw	0x0F
	movwf	BUF_I2C0
	movlw	0xF0
	skpc
	iorwf	BUF_I2C0, f

;add temporary to accumulator
	movf	TEMP0, w
	addwf	BUF_I2C0, f

;shift accumulator right 1 times
	clrc
	rrf		BUF_I2C0, f

	movfw	BUF_I2C0
  	addlw 	.155       
  	btfsc	STATUS,C
 	goto    HIGHER_Q_100_WHEEL
	movfw	BUF_I2C0
	goto	SEND_INFO_I2C

HIGHER_Q_100_WHEEL
	bsf		FLAGS_COM,STOP_ANG
	movlw	0xFF
	goto	SEND_INFO_I2C


;//**********************/**/ CAL_ANGLE_CENTER /**/  ****************************//
;//*******/**/To calculate the distance to travel, multiply by 0.9433 /**/*********//
;//*******************************************************************************//
CAL_ANGLE_CENTER

	btfsc	FLAGS_COM,STOP_ANG
	goto	HIGHER_Q_100_CENTER

	movfw	DIST_AUX_R
	movwf	BUF_I2C0

;copy accumulator to temporary
	movf	BUF_I2C0, w
	movwf	TEMP0

;shift accumulator right 1 times
	clrc
	rrf		BUF_I2C0, f

;add temporary to accumulator
	addwf	BUF_I2C0, f

;shift accumulator right 4 times
	swapf	BUF_I2C0, w
	andlw	0x0F
	movwf	BUF_I2C0
	skpnc
	bsf	BUF_I2C0, 4

;substract temporary from accumulator
	movf	TEMP0, w
	subwf	BUF_I2C0, f

;shift accumulator right 4 times
	swapf	BUF_I2C0, w
	andlw	0x0F
	movwf	BUF_I2C0
	movlw	0xF0
	skpc
	iorwf	BUF_I2C0, f

;add temporary to accumulator
	movf	TEMP0, w
	addwf	BUF_I2C0, f

	movfw	BUF_I2C0

  	addlw 	.155       
    btfsc	STATUS,C
  	goto    HIGHER_Q_100_CENTER
	movfw	BUF_I2C0
	goto	SEND_INFO_I2C

HIGHER_Q_100_CENTER
	bsf		FLAGS_COM,STOP_ANG
	movlw	0xFF
	goto	SEND_INFO_I2C