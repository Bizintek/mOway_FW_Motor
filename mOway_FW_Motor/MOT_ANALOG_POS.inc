;########################################################################################
;#####################################[FLANK_DETECTOR_L]#################################
;########################################################################################
;****************************************************************************************
;*	Description:													 					*
;*		Detects flanks on the left encoder							        			*
;****************************************************************************************
;*Variables involved:															        *
;*		CONTROL_FLAGS,HIGH_LOW_L					;Control flags to know where we're  *
;*													     							    *
;*		CONTROL_FLAGS,FLANK_L_DET					;Flag to indicate that FLANK has    *
;*													;been detected  					*
;*		CONTROL_FLAGS,FIRST_CRONO_L					;If the first one is rising edge	*
;*													;CRONO is reset  					*
;*		CRONO_L 									;Crono								*
;########################################################################################
;########################################################################################

FLANK_DETECTOR_L

	;It is checked if the first reading was made for the flag CONTROL_FLAGS, HIGH_LOW_L
	btfsc	CONTROL_FLAGS,FIRST_READ_L
	goto	FLANK_DETECTOR_L_NO_FIRST
	;The first reading has not been done
		
	call	READ_ANALOG_LEFT

FLANK_DETECTOR_L_FIRST_INF
	;***DEBUG
	subwf	LIMIT_LOW_L,W								;* if LIMIT_LOW_L - W >= 0 -> C = 1
	banksel	STATUS
	;*********************
	btfss	STATUS,C
	goto	FLANK_DETECTOR_L_FIRST_SUP					;* High level
	bcf		CONTROL_FLAGS,HIGH_LOW_L					;* Low level
	bsf		CONTROL_FLAGS,FIRST_READ_L
	return

FLANK_DETECTOR_L_FIRST_SUP
	movfw	ANALOG_LEFT
	;***DEBUG	
	subwf	LIMIT_HIGH_L,W										;* if LIMIT_HIGH_L - W >= 0 -> C = 1
	banksel	STATUS
	;**************************
	btfsc	STATUS,C
	return
	bsf		CONTROL_FLAGS,HIGH_LOW_L					;* High level
	bsf		CONTROL_FLAGS,FIRST_READ_L
	return



FLANK_DETECTOR_L_NO_FIRST
	;The analog value of the encoder is read
	call	READ_ANALOG_LEFT
	;Depending on where we are, it is compared in one way or another
	btfss	CONTROL_FLAGS,HIGH_LOW_L
	goto	FLANK_DETECTOR_L_LOW
FLANK_DETECTOR_L_HIGH
	;***DEBUG
	subwf	LIMIT_LOW_L,W			;* The KRC level is high
	banksel	STATUS	
	;*****************
	btfss	STATUS,C
	return
	;There has been a change and we update the control flag
	bcf		CONTROL_FLAGS,HIGH_LOW_L
	;Indicates that a flank has been detected
	bsf		CONTROL_FLAGS_2,FLANK_LOW_L_DET

	return
FLANK_DETECTOR_L_LOW
	;***DEBUG	
	subwf	LIMIT_HIGH_L,W
	banksel	STATUS
	;**********************************
	btfsc	STATUS,C
	return
	;There has been a change and we update the control flag
	bsf		CONTROL_FLAGS,HIGH_LOW_L
	;Indicates that a flank has been detected
	bsf		CONTROL_FLAGS_2,FLANK_HIGH_L_DET
	return


;########################################################################################
;#####################################[FLANK_DETECTOR_R]#################################
;########################################################################################
;****************************************************************************************
;*	Description:																		*
;*		Detects flanks on the right encoder							    				*
;****************************************************************************************
;*Variables involved:											         				*
;*		CONTROL_FLAGS,HIGH_LOW_R					;Control flags to know where we're  *
;*													        							*
;*		CONTROL_FLAGS,FLANK_R_DET					;Flag to indicate that FLANK has    *
;*													;been detected	                    *
;########################################################################################
;########################################################################################

FLANK_DETECTOR_R

	;It is checked if the first reading was made for the flag CONTROL_FLAGS, HIGH_LOW_R
	btfsc	CONTROL_FLAGS,FIRST_READ_R
	goto	FLANK_DETECTOR_R_NO_FIRST
	;The first reading has not been made
	call	READ_ANALOG_RIGHT
FLANK_DETECTOR_R_FIRST_INF
	;***DEBUG
	subwf	LIMIT_LOW_R,W
	banksel	STATUS
	;***************************
	btfss	STATUS,C
	goto	FLANK_DETECTOR_R_FIRST_SUP
	bcf		CONTROL_FLAGS,HIGH_LOW_R
	bsf		CONTROL_FLAGS,FIRST_READ_R
	return
FLANK_DETECTOR_R_FIRST_SUP
	movfw	ANALOG_RIGHT
	;***DEBUG		
	subwf	LIMIT_HIGH_R	,W
	banksel	STATUS
	;************************
	btfsc	STATUS,C
	return
	bsf		CONTROL_FLAGS,HIGH_LOW_R
	bsf		CONTROL_FLAGS,FIRST_READ_R
	return


FLANK_DETECTOR_R_NO_FIRST
	;The analog value of the encoder is read
	call	READ_ANALOG_RIGHT
	;Depending on where we are, it is compared in one way or another
	btfss	CONTROL_FLAGS,HIGH_LOW_R
	goto	FLANK_DETECTOR_R_LOW
FLANK_DETECTOR_R_HIGH
	;It is compared to the upper limit
	;***DEBUG
	subwf	LIMIT_LOW_R,W
	banksel	STATUS
	;********************
	btfss	STATUS,C
	return
	;There has been a change and we update the control flag
	bcf		CONTROL_FLAGS,HIGH_LOW_R
	;It is indicated that a falling edge has been detected
	bsf		CONTROL_FLAGS_2,FLANK_LOW_R_DET
	return
FLANK_DETECTOR_R_LOW
	;It is compared to the upper limit
	;***DEBUG	
	subwf	LIMIT_HIGH_R	,W
	banksel	STATUS
	;********************
	btfsc	STATUS,C
	return
	;There has been a change and we update the control flag
	bsf		CONTROL_FLAGS,HIGH_LOW_R
	;Indicates that a flank has been detected
	bsf		CONTROL_FLAGS_2,FLANK_HIGH_R_DET

	return


;########################################################################################
;####################################[DISTANCE_MANAGER_L]###############################
;########################################################################################
;****************************************************************************************
;*	Description:																		*
;*		Increase odometer L and the variable of auxiliary distance 						*
;*		for INFO command and if the command is distnace calls to STOP.	                *
;*		 																		        *
;****************************************************************************************
;*Variables involved:       															*
;*		CONTROL_FLAGS,FLANK_L_DET	;Flag detection FLANK. 							    *
;*									VERY IMPORTANT RESET    							*
;*		DISTANCE_L					;Distance to travel				    				*
;*		DIST_AUX_L					;Partial distance                                   *
;*		FLAGS_PWM,KILO_L			;Indicates motor sense								*
;*		KILO_L_L					;Variable L of the account							*
;*		KILO_L_H					;Variable H of the account							*
;########################################################################################
;########################################################################################
DISTANCE_MANAGER_L
	;We reset flag of detection FLANK
	bcf		CONTROL_FLAGS_2,FLANK_HIGH_L_DET
	bcf		CONTROL_FLAGS_2,FLANK_LOW_L_DET

	btfsc	FLAGS_COM,COM_DIST_L
	goto	DISTANCE_MANAGER_L_INFO
	decfsz	DISTANCE_L
	goto	DISTANCE_MANAGER_L_INFO
	call	STOP	
	return

DISTANCE_MANAGER_L_INFO
	incf	DIST_AUX_L,F				;For distance and temporal angle

	btfss	FLAGS_PWM,KILO_L			;General odometer L when
	goto	ODOMETER_L_00				;the two engines go in the same direction:
	incfsz	KILO_L_L,F					;16 bit counter.
	goto	ODOMETER_L_00
	incf	KILO_L_H,F
	
ODOMETER_L_00							;General odometer L when
	btfsc	FLAGS_PWM,KILO_L			;the engines go in the opposite direction
	return								;16 bit driver.
	decf	KILO_L_L,F
	comf	KILO_L_L,W					;dec doesn't change STATUS, C.
	btfss	STATUS,Z
	return
	decf	KILO_L_H,F
	return
;#######################################################################################






;########################################################################################
;#####################################[DISTANCE_MANAGER_R]##############################
;########################################################################################
;****************************************************************************************
;*	Description:																		*
;*		Increase odometer R and the variable of auxiliary distance 						*
;*		for INFO command and if the command is distnace calls to STOP.                  *
;*										             									*
;****************************************************************************************
;*Variables involved:       															*
;*		CONTROL_FLAGS,FLANK_R_DET	;Flag detection FLANK. 						    	*
;*									VERY IMPORTANT RESET                                *
;*		DISTANCE_R					;Distance to travel							     	*
;*		DIST_AUX_R					;Partial distance		     						*
;*		FLAGS_PWM,KILO_R			;Indicates motor sense					    		*
;*		KILO_R_L					;Variable L of the account							*
;*		KILO_R_H					;Variable H of the account							*
;########################################################################################
;########################################################################################
DISTANCE_MANAGER_R

	;We reset flag of detection FLANK
	bcf		CONTROL_FLAGS_2,FLANK_HIGH_R_DET
	bcf		CONTROL_FLAGS_2,FLANK_LOW_R_DET

	btfsc	FLAGS_COM,COM_DIST_R
	goto	DISTANCE_MANAGER_R_INFO
	decfsz	DISTANCE_R	
	goto	DISTANCE_MANAGER_R_INFO
	call	STOP	
	return

DISTANCE_MANAGER_R_INFO
	incf	DIST_AUX_R,F				;For distance and temporal angle

	btfss	FLAGS_PWM,KILO_R			;General odometer R when
	goto	DISTANCE_MANAGER_R_00		;the two engines go in the same direction:
	incfsz	KILO_R_L,F					;16 bit counter.
	goto	DISTANCE_MANAGER_R_00
	incf	KILO_R_H,F

DISTANCE_MANAGER_R_00					;General odometer R when
	btfsc	FLAGS_PWM,KILO_R			;the engines go in the opposite direction
	return								;16 bit driver.
	decf	KILO_R_L,F
	comf	KILO_R_L,W					;dec doesn't change STATUS, C. 
	btfss	STATUS,Z
	return
	decf	KILO_R_H,F

	return
;#######################################################################################




;########################################################################################
;#####################################[COUNT_WIDTH_L]#################################
;########################################################################################
;****************************************************************************************
;*	Description:																		*
;*		Counter to count pulse width. Enter every 140us approx depends on the           *
;*		interruptions of TIMER and I2C (140us the two functions of pulse detector).     *
;*										                                                *
;****************************************************************************************
;* Variables involved: 										         					*
;*		CRONO_L						;Counter variable	 					    		*
;########################################################################################
;########################################################################################
COUNT_WIDTH_L
	;Sum the crono
	incfsz	CRONO_L
	return
	;if it is exceeded, 0xFF is maintained
	decf	CRONO_L,F
	return

;########################################################################################
;#####################################[COUNT_WIDTH_R]#################################
;########################################################################################
;****************************************************************************************
;*	Description:																		*
;*		Counter to count pulse width. Enter every 140us approx depends on the           *
;*		interruptions of TIMER and I2C (140us the two functions of pulse detector).     *										*
;****************************************************************************************
;*Variables involved:														         	*
;*		CRONO_R							;Counter variable	 					    	*
;########################################################################################
;########################################################################################
COUNT_WIDTH_R

	;Sum the crono
	incfsz	CRONO_R	
	return
	;if it is exceeded, 0xFF is maintained
	decf	CRONO_R	,F
	return

;########################################################################################
;#####################################[ACTED_R]############################################
;########################################################################################
;****************************************************************************************
;*	Description:																		*
;*		Depending on the width of the pulse, the PWM is actuated						*
;****************************************************************************************
;*Variables involved:														        	*
;*		CRONO_R								;Counter variable	 						*
;*		THIGH_R							;Value of the PWM where we act					*
;*		CONTROL_FLAGS,FLANK_HIGH_R_DET  ;FLANK uploaded detected-> Act			        *
;*		CONTROL_FLAGS,FIRST_CRONO_R	 ;Indicates if it is the first crono of the command *
;*		CRONO_R								;If it's the first time I act when the  	*
;*										;robot is standing, I reject CRONO because I 	*
;*										;don't know where has started counting.  		*
;########################################################################################
;########################################################################################
ACTED_R

	;It checks if FLANK has been detected
	btfss	CONTROL_FLAGS_2,FLANK_HIGH_R_DET	
	return	

	;If it is the first reading from the standpoint, we reject it
	btfsc	CONTROL_FLAGS,FIRST_CRONO_R	;Check if it is first reading
	goto	ACT_R_NO_FIRST			
	bsf		CONTROL_FLAGS,FIRST_CRONO_R	
	clrf	CRONO_R	
	return
	
ACT_R_NO_FIRST
					
	movfw	CRONO_R					
	subwf	SPEED_R,W		;The CRONO is subtracted from the setpoint calculated in I2C
	btfsc	STATUS,C
	goto	GO_DOWN_PWM_R

GO_UP_PWM_R						;Faster
	clrf	CRONO_R	
	movwf	DIFF_R				;We calculate the difference
	comf	DIFF_R,F
	clrc
	rrf		DIFF_R,F			;Kp=0,5
	movfw	DIFF_R
	subwf	THIGH_R,W			;Check if the subtraction is less than 0
	btfss	STATUS,C
	goto	GO_UP_PWM_R_LIMIT
	sublw	SPEED_MAX_PWM			;It is checked if the sample is less than the maximum
	btfsc	STATUS,C
	goto	GO_UP_PWM_R_LIMIT
	movfw	DIFF_R
	subwf	THIGH_R,F			;You have to decrease to increase the duty of the PWM
	return

GO_DOWN_PWM_R						;Slower
	btfsc	STATUS,Z			;If they are equal, we do nothing
	goto	ACT_R_RETURN
	clrf	CRONO_R	
	movwf	DIFF_R				;The proportion is calculated
	clrc
	rrf		DIFF_R,F			;Kp=0,5		
	addwf	THIGH_R,W
	btfsc	STATUS,C			;Check if sum is greater than 255
	goto	GO_DOWN_PWM_R_LIMIT
	sublw	SPEED_MIN_PWM
	btfss	STATUS,C			;It is checked if the sample is greater than the minimum
	goto	GO_DOWN_PWM_R_LIMIT
	movfw	DIFF_R
	addwf	THIGH_R,F			;You have to decrease to increase the duty of the PWM
	return

ACT_R_RETURN
	clrf	CRONO_R	
	return

GO_UP_PWM_R_LIMIT				;You have to put the PWM in the limit
	movlw	SPEED_MAX_PWM
	movwf	THIGH_R				;Duty in the limit
	return

GO_DOWN_PWM_R_LIMIT				;You have to put the PWM in the limit
	movlw	SPEED_MIN_PWM
	movwf	THIGH_R				;Duty in the limit
	return


;########################################################################################
;#####################################[ACTED_L]########################################
;########################################################################################
;****************************************************************************************
;*	Description:																		*
;*		Depending on the width of the pulse, the PWM is actuated						*
;****************************************************************************************
;*Variables involved:												        			*
;*		CRONO_L							;Counter variable                               *
;*		THIGH_L							;Value of the PWM where we act					*
;*		CONTROL_FLAGS,FLANK_HIGH_L_DET  ;FLANK uploaded detected-> Act      			*
;*		CONTROL_FLAGS,FIRST_CRONO_L	;Indicates if it is the first crono of the command	*
;*		CRONO_L							;If it's the first time I act when the  	    *
;*										;robot is standing, I reject CRONO because I 	*
;*										;don't know where has started counting.     	*
;########################################################################################
;########################################################################################
ACTED_L

	btfss	CONTROL_FLAGS_2,FLANK_HIGH_L_DET
	return						;It checks if FLANK has been detected

	;If it is the first reading from the standpoint, we reject it
	btfsc	CONTROL_FLAGS,FIRST_CRONO_L
	goto	ACT_L_NO_FIRST
	bsf		CONTROL_FLAGS,FIRST_CRONO_L
	clrf	CRONO_L
	return
	
ACT_L_NO_FIRST

	movfw	CRONO_L				
	subwf	SPEED_L,W			;The CRONO is subtracted from the setpoint calculated in I2C		
	btfsc	STATUS,C
	goto	GO_DOWN_PWM_L

GO_UP_PWM_L						;Faster
	clrf	CRONO_L
	movwf	DIFF_L				;The proportion is calculated
	comf	DIFF_L,F
	clrc
	rrf		DIFF_L,F			;Kp=0,5
	movfw	DIFF_L
	subwf	THIGH_L,W			;Check if the subtraction is less than 0
	btfss	STATUS,C
	goto	GO_UP_PWM_L_LIMITE
	sublw	SPEED_MAX_PWM		;It is checked if the sample is less than the maximum
	btfsc	STATUS,C
	goto	GO_UP_PWM_L_LIMITE
	movfw	DIFF_L
	subwf	THIGH_L,F			;You have to decrease to increase the duty of the PWM
	return

GO_DOWN_PWM_L					;Slower
	btfsc	STATUS,Z			;If they are equal, we do nothing
	goto	ACT_L_RETURN
	clrf	CRONO_L
	movwf	DIFF_L				;The proportion is calculated
	clrc
	rrf		DIFF_L,F			;Kp=0,5
	addwf	THIGH_L,W	
	btfsc	STATUS,C			;It is checked if the sum is greater than 255
	goto	GO_DOWN_PWM_L_LIMITE
	sublw	SPEED_MIN_PWM
	btfss	STATUS,C			;It is checked if the sample is greater than the minimum
	goto	GO_DOWN_PWM_L_LIMITE
	movfw	DIFF_L
	addwf	THIGH_L,F			;You have to decrease to increase the duty of the PWM
	return

ACT_L_RETURN
	clrf	CRONO_L
	return

GO_UP_PWM_L_LIMITE				;You have to put the PWM in the limit
	movlw	SPEED_MAX_PWM
	movwf	THIGH_L				;Duty in the limit
	return

GO_DOWN_PWM_L_LIMITE			;You have to put the PWM in the limit
	movlw	SPEED_MIN_PWM
	movwf	THIGH_L				;Duty in the limit
	return


;########################################################################################
;#####################################[STOP_DELAY]######################################
;########################################################################################
;****************************************************************************************
;*	Description:																		*
;*		Delay that is executed when braking is performed								*
;****************************************************************************************
;*Variables involved:											        				*
;*		SPEED_L							;Speed L to check that V is less	            *
;*		SPEED_R							;Speed R to check that V is less                *
;*		CONTROL_FLAGS,STOP_INIT		 	;Indicates when we should enter the delay       *
;*											;change in STOP and MOVE (if a new command  *
;*											;arrives you have to exit the delay)		*
;*		INERTIA_DELAY						;Variable that indicates how many more delay*
;*											;depending on the minimum spped.    		*
;########################################################################################
;########################################################################################


STOP_DELAY
	btfss		CONTROL_FLAGS,STOP_INIT
	return
	;17ms VEL=1. Minimum time
	call		DELAY_17ms
	;You have to calculate which speed is higher
	movfw		SPEED_L
	subwf		SPEED_R,W
	movfw		SPEED_L
	btfss		STATUS,C	
	movfw		SPEED_R

	sublw		SPEED_MIN
	movwf		INERTIA_DELAY
	
	btfss		CONTROL_FLAGS,STOP_INIT		;If a MOVE command has arrived while 
											;we are in the delay it comes out.
	return
	call		Delay_130us
	decfsz		INERTIA_DELAY,F
	goto		$-4

	btfss		CONTROL_FLAGS,STOP_INIT		;If a MOVE command has arrived while 
											;we are in the delay it comes out.
	return
	call		STOP_INERTIA
	return


;########################################################################################
;#####################################[READ_ANALOG_LEFT]#################################
;########################################################################################
;****************************************************************************************
;*	Description:																		*
;*		Read the 8-bit analog value of the left encoder by saving it in		        	*
;*		ANALOG_LEFT. 50us takes.														*
;****************************************************************************************
;*Variables involved:													        		*
;*		ANALOG_LEFT					;Partial distance				     				*
;*																						*
;*																						*
;*Execution time: 59us												     				*
;########################################################################################
;########################################################################################
READ_ANALOG_LEFT
	BANKSEL	ADCON1
	bsf		STATUS,RP0
	MOVLW	b'01010000'	;ADC Frc clock
	MOVWF	ADCON1		;
	bcf		STATUS,RP0	;
	MOVLW	b'00011101'	;Left justify,
	MOVWF	ADCON0		; Vdd Vref, AN7, On
	CALL	Delay		;Acquisiton delay
	BSF		ADCON0,GO	;Start conversion
	BTFSC	ADCON0,GO	;Is conversion done?
	GOTO	$-1			;No, test again
	MOVF	ADRESH,W	;Read upper 2 bits
	MOVWF	ANALOG_LEFT	;store in GPR space
	
	return
;########################################################################################





;########################################################################################
;#####################################[READ_ANALOG_RIGHT]################################
;########################################################################################
;****************************************************************************************
;*	Description:																		*
;*		Read the 8-bit analog value of the right encoder by saving it in	 			*
;*		ANALOG_RIGHT. 50us takes.														*
;****************************************************************************************
;*Variables involved:		         													*
;*		ANALOG_RIGHT					;Partial distance								*
;*																						*
;*																						*
;*Execution time: 59us																	*
;########################################################################################
;########################################################################################
READ_ANALOG_RIGHT
	BANKSEL	ADCON1		;
	bsf		STATUS,RP0
	MOVLW	b'01010000'	;ADC Frc clock
	MOVWF	ADCON1		;
	bcf		STATUS,RP0	;
	MOVLW	b'00010001'	;Left justify,
	MOVWF	ADCON0		; Vdd Vref, AN4, On
	CALL	Delay		;Acquisiton delay
	BSF		ADCON0,GO	;Start conversion
	BTFSC	ADCON0,GO	;Is conversion done?
	GOTO	$-1			;No, test again
	MOVF	ADRESH,W	;Read upper 2 bits
	MOVWF	ANALOG_RIGHT  ;store in GPR space
	return
;########################################################################################




;*************************************Delays************************************************
;Delay 30uS
Delay
	;55 cycles
	movlw	0x12
	movwf	DELAY_00
Delay_0
	decfsz	DELAY_00, f
	goto	Delay_0
	nop
	return
;*******************
;Delay 300uS aprox.
Delay_300us
			;55 cycles
	movlw	0xDD
	movwf	DELAY_00
Delay_300us_0
	decfsz	DELAY_00, f
	goto	Delay_300us_0
	nop
	return

;*******************
Delay_1.5s
	;2999995 cycles
	movlw	0x1A
	movwf	DELAY_00
	movlw	0x8B
	movwf	DELAY_01
	movlw	0x07
	movwf	DELAY_02
Delay_1.5s_0
	decfsz	DELAY_00, f
	goto	$+2
	decfsz	DELAY_01, f
	goto	$+2
	decfsz	DELAY_02, f
	goto	Delay_1.5s_0
			;1 cycle
	nop
			;4 cycles (including call)
	return

;*******************

DELAY_100ms
			;199993 cycles
	movlw	0x3E
	movwf	DELAY_00
	movlw	0x9D
	movwf	DELAY_01
DELAY_100ms_0
	decfsz	DELAY_00, f
	goto	$+2
	decfsz	DELAY_01, f
	goto	DELAY_100ms_0

			;3 cycles
	goto	$+1
	nop

			;4 cycles (including call)
	return

;Delay 130uS 
Delay_130us
			;55 cycles
	movlw	0x55
	movwf	DELAY_00
Delay_130us_0
	decfsz	DELAY_00, f
	goto	Delay_130us_0
	nop
	return


DELAY_17ms
			;199993 cycles
	movlw	0x8E
	movwf	DELAY_00
	movlw	0x1B
	movwf	DELAY_01
DELAY_17ms_0
	decfsz	DELAY_00, f
	goto	$+2
	decfsz	DELAY_01, f
	goto	DELAY_17ms_0

			;3 cycles
	goto	$+1
	nop

			;4 cycles (including call)
	return


;########################################################################################
;#####################################[READ_EEPROM]######################################
;########################################################################################
;****************************************************************************************
;*	Description:																		*
;*	In W, take the address to read and return											*
;*	with the value in W.																*
;****************************************************************************************
;*Variables involved:			                   										*
;*																						*
;*																						*
;*																						*
;*																						*
;########################################################################################
;########################################################################################

;*******************************************RS232_SEND************************************************

RS232_SEND
	banksel	TXREG  
	movwf 	TXREG           ; send data in W

	banksel	TXSTA          	; RAM PAGE 1
WtHere  
	btfss 	TXSTA,TRMT      ; (1) transmission is complete if hi
    goto 	WtHere

	bcf		STATUS,RP0
	bcf		STATUS,RP1
	return

