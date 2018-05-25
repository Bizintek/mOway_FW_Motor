;-------------------------------------
;---------	New engine	--------------
;-------------------------------------
;*************************************
;*	     Engine control system       *
;*************************************
;*		I2C	->	Hardware			 *
;*		PWM	->	TMR0 y TMR1			 *
;*      POS ->  MOT_ANALOG_POS		 *
;*************************************
;*	Versión:	5.0.1                *
;*************************************
;****************************************************************
;*	Description:												*
;*		This program manages the two engines of the microbot	*
;*	by controlling the speed by PWM. Control the position      	*
;*	of the robot with encoders.						     		*
;*		Receives I2C commands from the main microbot PIC and	*
;*	notifies them when finish the execution of that commands.   *
;*												            	*
;****************************************************************
;****************************************************************
;*	Modification:												*
;*		The analog value of the level has been changed 			*
;*		of the sensors of the encoders. Definition in    		*
;* 		"def_pos.inc"											*
;****************************************************************
;****************************************************************
;*	Code: Production		*		State: Checked   			*
;****************************************************************
;****************************************************************
;___________________________________________________________________________


	LIST P=16F687

	#include "P16F687.INC"
	__config 0x30F4       
	#include "def_pwm.inc"
	#include "def_i2c.inc"
	#include "def_pos.inc"
	#include "def_tune.inc"
	
	

;************************************
;*	     Vector declaration   		*
;************************************

	ORG	0x00		;Reset vector
	goto	INICIO

	ORG	0x04		;Interruption vector
	goto	INTER
;************************************
;*	        Program memory          *
;************************************
	
	ORG	0x05	
INICIO


	#include "conf_pwm_i2c.inc"


;*	Initialization of the system

	;It starts from a 50% PWM DUTY
	movlw	.128
	movwf	THIGH_R
	movwf	THIGH_L

	clrf	CONTROL_FLAGS				;The control flags are cleaned

	clrf	CONT_TIME_H	
	clrf	CONT_TIME_L
	clrf	FLAGS_I2C
	clrf	FLAGS_COM
	clrf	COMMAND
	clrf	SPEED_AUX
	clrf	TYPE_ROTATE
	clrf	BUF_I2C	
	clrf	N
	clrf	KILO_L_L
	clrf	KILO_L_H
	clrf	KILO_R_L
	clrf	KILO_R_H
	clrf	AVERAGE_KILO_H
	clrf	AVERAGE_KILO_L
	clrf	AVERAGE_KILO_H_TOTAL
	clrf	AVERAGE_KILO_L_TOTAL

	clrf	BUF_I2C0	
	clrf 	BUF_I2C1	
	clrf 	TEMP0		
	clrf	TEMP1		

	clrf	CON_STATUS	
	clrf	CON_ACT

	movlw	SPEED_MIN
	movwf	SPEED_L
	movwf	SPEED_R

	;The encoders are activated
	bsf		PORTA,ENCODER_L_EN
	bsf		PORTB,ENCODER_R_EN

	;It is indicated that the motor is configured
	bsf		PORTC,FINISHED

	; Read stored KRC values from EEPROM
	call	READ_TUN
	
	;**************************************************************
	;		Main program
	;**************************************************************
	
	
MAIN

	btfss	CONTROL_FLAGS,MOVE_INIT				;If the command has finished, it remains here
	goto	$-1
	
	;Flank detectors
	call	FLANK_DETECTOR_R
	;Pulse width counter
	call	COUNT_WIDTH_R
	;Actuator when you have a rising edge
	call	ACTED_R
	;Distance manager
	btfsc	CONTROL_FLAGS_2,FLANK_LOW_R_DET	
	call	DISTANCE_MANAGER_R
	btfsc	CONTROL_FLAGS_2,FLANK_HIGH_R_DET	
	call	DISTANCE_MANAGER_R


	call	FLANK_DETECTOR_L
	;Pulse width counter
	call	COUNT_WIDTH_L
	;Actuator when you have a rising edge
	call	ACTED_L
	;Distance manager
	btfsc	CONTROL_FLAGS_2,FLANK_LOW_L_DET
	call	DISTANCE_MANAGER_L
	btfsc	CONTROL_FLAGS_2,FLANK_HIGH_L_DET
	call	DISTANCE_MANAGER_L
	
	call	STOP_DELAY

	goto	MAIN

;###################################################################################
;###########################[ROUTINES OF INTERRUPTION]##############################
;###################################################################################

INTER
	bcf		INTCON,GIE		;Disables interrupts	
	bcf		INTCON,PEIE

	movwf 	W_TEMP 			;Copy W to the TEMP record
	swapf 	STATUS,W 		;Swap is done so as not to modify the STATUS values. If STATUS is done, W changes the Z
	clrf 	STATUS 			;Move to BANK0
	movwf 	STATUS_TEMP 	;Move STATUS to the STATUS_TEMP of BANK0BANK0
	
	banksel	PIR1
	btfsc	PIR1,TMR1IF
	call	INT_TMR1		;Interruption by TMR1 (MOT_PWM.inc)

	banksel	INTCON
	btfsc	INTCON,T0IF
	call	INT_TMR0		;Interruption by TMR0 (MOT_PWM.inc)


	banksel PIR1
	btfsc	PIR1,SSPIF
	call	INT_I2C			;Interruption by I2C (MOT_I2C.inc)
	
	swapf 	STATUS_TEMP,W 	;With this you have STATUS in W without changing STATUS
							;(It is left in the bank that was)
	movwf 	STATUS 			;Move W (STATUS_TEMP) to STATUS
	swapf 	W_TEMP,F 		;Change W to then put it in W without modifying STATUS
	swapf 	W_TEMP,W 		;Swap W_TEMP into W

	bsf		INTCON,PEIE
	bsf		INTCON,GIE		;Enable interruptions
	retfie

	#include "MOT_ANALOG_POS.inc" 	;Routines calculate position with analog encoder
	#include "MOT_PWM.inc"			;Routines in charge of the PWM
	#include "MOT_I2C.inc"			;Routines in charge of I2C
	#include "MOT_MATH.inc"			;Routines that calculate angles
	#include "MOT_TUNE.inc"			;
	
 END
