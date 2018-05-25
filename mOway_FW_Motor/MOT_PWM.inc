;********************[Timing INT_TMR0 20us]*****************************
INT_TMR0									;Interruption of the timer 0 for the left engine
											; Delay timer
	nop
	movlw	.6
	movwf	DELAY_PWM
	decfsz	DELAY_PWM
	goto	$-1

	bcf		STATUS,RP0
	bcf		STATUS,RP1
	btfss	INTCON,T0IE
	return
	bcf		INTCON,T0IF
	btfss	FLAGS_PWM,SENSE_L				;Check the sense
	goto	L_GO_BACK_PULSE					;It goes back
	goto	L_GO_AHEAD_PULSE				;It goes ahead


;********************[Timing INT_TMR1 20us]*****************************
INT_TMR1	
	bcf		STATUS,RP0
	bcf		STATUS,RP1						    ;Interruption of the timer 1 for the left engine
	goto	TIME_L
INT_TMR1_RETURN
	bsf		STATUS,RP0
	btfss	PIE1,TMR1IE							;If you have called to STOP here you will leave
	return
	bcf		STATUS,RP0
	bcf		PIR1,TMR1IF
	btfss	FLAGS_PWM,SENSE_R
	goto	R_GO_BACK_PULSE
	goto	R_GO_AHEAD_PULSE

;************************************
;*	         Time control			*
;************************************
;* Controls the time indicated for the command
;* The time is set in steps of 100ms
;* 15 instructions
TIME_L
	movfw	CONT_TIME_L
	sublw	.185
	btfsc	STATUS,Z
	goto	TIME_H
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	incf	CONT_TIME_L
	goto	INT_TMR1_RETURN
TIME_H
	clrf	CONT_TIME_L
	movfw	CONT_TIME_H
	sublw	.255
	btfsc	STATUS,Z
	goto	OVERFLOW_TIME
	incf	CONT_TIME_H
	btfss	FLAGS_COM,TYPE_COM
	goto	INT_TMR1_RETURN_2NOP	
	movfw	CONT_TIME_H
	subwf	TIME,W
	btfsc	STATUS,Z
	call	STOP
	goto	INT_TMR1_RETURN
OVERFLOW_TIME
	clrf	CONT_TIME_L
	clrf	CONT_TIME_H
	goto	INT_TMR1_RETURN

INT_TMR1_RETURN_2NOP
	goto	INT_TMR1_RETURN

;;***********************************
;*	 Subroutines of the PWM			*
;************************************
;***

L_GO_AHEAD_PULSE
	btfss	FLAGS_PWM,PULSE_L
	goto	T_HIGH_L_AD
	goto	T_LOW_L_AD	

L_GO_BACK_PULSE
	btfss	FLAGS_PWM,PULSE_L
	goto	T_HIGH_L_AT	
	goto	T_LOW_L_AT	

R_GO_AHEAD_PULSE
	btfss	FLAGS_PWM,PULSE_R
	goto	T_HIGH_R_AD
	goto	T_LOW_R_AD

R_GO_BACK_PULSE
	btfss	FLAGS_PWM,PULSE_R
	goto	T_HIGH_R_AT
	goto	T_LOW_R_AT



T_HIGH_L_AD
	bcf		PORTA,MOTOR_L_A
	nop
	nop
	movfw	THIGH_L
	movwf	TMR0
	bsf		FLAGS_PWM,PULSE_L
	btfsc	FLAGS_PWM,SPEED_0_L			;If the speed is 0, PWM is not removed
	return
	bsf		PORTA,MOTOR_L_B
	
	return

T_LOW_L_AD
	bcf		PORTA,MOTOR_L_A
	nop
	nop
	comf	THIGH_L,W
	movwf	TMR0
	bcf		FLAGS_PWM,PULSE_L
	bcf		PORTA,MOTOR_L_B
	return

T_HIGH_R_AD
	bcf		PORTC,MOTOR_R_A
	movlw	0xFF
	movwf	TMR1H
	movfw	THIGH_R
	movwf	TMR1L
	bsf		FLAGS_PWM,PULSE_R
	btfsc	FLAGS_PWM,SPEED_0_R			;If the speed is 0, PWM is not removed
	return
	bsf		PORTC,MOTOR_R_B
	
	return

T_LOW_R_AD
	bcf		PORTC,MOTOR_R_A
	movlw	0xFF
	movwf	TMR1H
	comf	THIGH_R,W
	movwf	TMR1L
	bcf		FLAGS_PWM,PULSE_R
	bcf		PORTC,MOTOR_R_B
	return

;***

T_HIGH_L_AT
	bcf		PORTA,MOTOR_L_B
	nop
	nop
	movfw	THIGH_L
	movwf	TMR0
	bsf		FLAGS_PWM,PULSE_L
	btfsc	FLAGS_PWM,SPEED_0_L			;If the speed is 0, PWM is not removed
	return
	bsf		PORTA,MOTOR_L_A
	return

T_LOW_L_AT
	bcf		PORTA,MOTOR_L_B
	nop
	nop
	comf	THIGH_L,W
	movwf	TMR0
	bcf		FLAGS_PWM,PULSE_L
	bcf		PORTA,MOTOR_L_A
	return

T_HIGH_R_AT
	bcf		PORTC,MOTOR_R_B
	movlw	0xFF
	movwf	TMR1H
	movfw	THIGH_R
	movwf	TMR1L
	bsf		FLAGS_PWM,PULSE_R
	btfsc	FLAGS_PWM,SPEED_0_R			;If the speed is 0, PWM is not removed
	return
	bsf		PORTC,MOTOR_R_A
	return

T_LOW_R_AT
	bcf		PORTC,MOTOR_R_B
	movlw	0xFF
	movwf	TMR1H
	comf	THIGH_R,W
	movwf	TMR1L
	bcf		FLAGS_PWM,PULSE_R
	bcf		PORTC,MOTOR_R_A
	return

;##########Start PWM############
MOVE									;Every time a movement command is finished
										;it comes here
	;The encoders are activated
	banksel	PORTA
	
	clrf	CONT_TIME_L				;The TIME counter is cleared
	clrf	CONT_TIME_H				
	clrf	DIST_AUX_R					;The distance traveled by the command is cleared
	clrf	DIST_AUX_L
	;It is initialized when we are stopped. But the data is valid
	bcf		CONTROL_FLAGS,STOP_INIT	;If you are in the delay you have to leave
	btfsc	CONTROL_FLAGS,MOVE_INIT
	goto	CONTROL_CHECK_SPEED_RST_L
	clrf	CONTROL_FLAGS				;The control flags are cleaned
	clrf	CONTROL_FLAGS_2
CONTROL_CHECK_SPEED_RST_L
	
	;You have to check if the wheels were stopped
	btfss	FLAGS_PWM,SPEED_0_L
	goto	CONTROL_CHECK_SPEED_RST_R
	bcf		CONTROL_FLAGS,FIRST_READ_L
	bcf		CONTROL_FLAGS,HIGH_LOW_L
	bcf		CONTROL_FLAGS,FIRST_CRONO_L
	bcf		CONTROL_FLAGS_2,FLANK_HIGH_L_DET
	bcf		CONTROL_FLAGS_2,FLANK_LOW_L_DET
CONTROL_CHECK_SPEED_RST_R
	btfss	FLAGS_PWM,SPEED_0_R
	goto	CONTROL_CHECK_SPEED_RST_CONT
	bcf		CONTROL_FLAGS,FIRST_READ_R
	bcf		CONTROL_FLAGS,HIGH_LOW_R
	bcf		CONTROL_FLAGS,FIRST_CRONO_R
	bcf		CONTROL_FLAGS_2,FLANK_HIGH_R_DET
	bcf		CONTROL_FLAGS_2,FLANK_LOW_R_DET

CONTROL_CHECK_SPEED_RST_CONT

	bsf		CONTROL_FLAGS,MOVE_INIT

	banksel	FLAGS_PWM
	bcf		FLAGS_PWM,SPEED_0_R			;Speed 0; Do not take 1 in the engine
	bcf		FLAGS_PWM,SPEED_0_L
	movfw	SPEED_R
	sublw	SPEED_MIN
	btfsc	STATUS,Z
	bsf		FLAGS_PWM,SPEED_0_R
	movfw	SPEED_L
	sublw	SPEED_MIN
	btfsc	STATUS,Z
	bsf		FLAGS_PWM,SPEED_0_L
										;General speedometer
	bsf		FLAGS_PWM,KILO_L			;Indicates whether to add or subtract
	bsf		FLAGS_PWM,KILO_R

	btfss	FLAGS_PWM,SENSE_L
	goto	KILO_0	
	btfss	FLAGS_PWM,SENSE_R
	bcf		FLAGS_PWM,KILO_R
	goto	$+3	
KILO_0
	btfsc	FLAGS_PWM,SENSE_R
	bcf		FLAGS_PWM,KILO_L
	
	movlw	0xFF						;Only 8 bits of the counter are used
	movwf	TMR1H						;They are initialized to 0xFF to enter after this function
	
	banksel	PIE1	
	bsf		PIE1,TMR1IE					;Interruption is enabled for PWM
	banksel	INTCON
	bsf		INTCON,T0IE					;Interruption is enabled for PWM

	bcf		PORTC,FINISHED

	return


	
;#############STOP PWM#########################
;*	Disable interrupts of timers
STOP				
	banksel	INTCON

	bcf		INTCON,T0IE					;Interruption timer0 is disabled
	banksel	PIE1
	bcf		PIE1,TMR1IE					;Interruption timer1 is disabled
	
	bcf		STATUS,RP0
	bcf		STATUS,RP1

	;Clean PWM interrupts
	bcf		PIR1,TMR1IF
	bcf		INTCON,T0IF

	;The engine is braked with a TIME dependent on the speed
STOP_CONT_00
	btfsc	FLAGS_PWM,SPEED_0_L
	goto	STOP_CONT_01
	bcf		PORTA,MOTOR_L_A
	bcf		PORTA,MOTOR_L_B
	btfsc	FLAGS_PWM,SENSE_L
	bsf		PORTA,MOTOR_L_A
	btfss	FLAGS_PWM,SENSE_L
	bsf		PORTA,MOTOR_L_B
STOP_CONT_01
	btfsc	FLAGS_PWM,SPEED_0_R
	goto	STOP_CONT_02
	bcf		PORTC,MOTOR_R_A
	bcf		PORTC,MOTOR_R_B
	btfsc	FLAGS_PWM,SENSE_R
	bsf		PORTC,MOTOR_R_A
	btfss	FLAGS_PWM,SENSE_R
	bsf		PORTC,MOTOR_R_B
STOP_CONT_02
	bsf		CONTROL_FLAGS,STOP_INIT
	return

STOP_INERTIA
	bsf		PORTA,MOTOR_L_A
	bsf		PORTA,MOTOR_L_B
	bsf		PORTC,MOTOR_R_A
	bsf		PORTC,MOTOR_R_B
	bcf		CONTROL_FLAGS,STOP_INIT
	bcf		CONTROL_FLAGS,MOVE_INIT
	bsf		PORTC,FINISHED
	return


STOP_COM
	banksel	INTCON

	bcf		INTCON,T0IE					;Interruption timer0 is disabled
	banksel	PIE1
	bcf		PIE1,TMR1IE					;Interruption timer1 is disabled
	
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

