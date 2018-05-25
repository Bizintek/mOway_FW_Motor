;************************************
;*	    I2C Subroutines				*
;************************************
;*	Determines whether the received byte is a command
;*	or a data associated with some I2C command.

;**********************[Timing INT_I2C]*******************************
;INFO queries: Variable time with a maximun of 40 us and min 14 us with 
;			   a frequency of about 300 us.
;				 (measured by asking KM and comparing with a constant in while inf)
;MOVE command: Variable time with maximum of 50us and min 14us with one 
;			   frequency of 300us.
;				 (measured by sending MOVE STR with mOwayGUI in while inf) 
;
; INFO queries: Variable time with a maximum of 40us and min 14us with
; a frequency of about 300 us
; (measured by asking KM and comparing with a constant in while inf)
; MOVE command: Variable time with maximum of 50us and min 14us with one
; frequency of 300us.
; (measured by sending MOVE STR with mOwayGUI in while inf)
; When adding one when the address arrives, the speed is balanced.

INT_I2C

	bcf		PIR1,SSPIF
	banksel	SSPCON						;It is checked if it is I2C reading command
	btfss	SSPCON,CKP					;If it is read it goes to STATE_SW
	goto	STATE_SW					;so that based on the previous command of
	btfsc	FLAGS_I2C,END_STATE			;writing, send the data to the PIC
	goto	FINAL_STATE

	bcf		STATUS,RP0
	bcf		STATUS,RP1	
	movfw	SSPBUF
	movwf	BUF_I2C						;First of all it takes the datas and they get into BUF_I2C	
										;ACK is sent automatically to the issuer
	banksel	SSPCON
	btfss	SSPCON,SSPOV
	goto	$+3
	bcf		SSPCON,SSPOV
	return

	banksel	SSPSTAT						;It is cleaned just in case (it is cleaned only when it is read)
	bcf		SSPSTAT,BF					;SSPBUF

	btfss	SSPSTAT,D_A
	goto	RECEIVE_ADDRESS				;The address of the engine module is received

	bcf		STATUS,RP0
	bcf		STATUS,RP1	

	btfsc	FLAGS_I2C,COM_DATA_INVAL
	return
				
	btfss	FLAGS_I2C,COMMAND_REC		;Look if you had a command before
	goto	IDENTIFY_COMMAND			;If you did not have it, you identify
	goto	RECEIVE_DATA				;If the command is known, the data that comes is collected

;********** [Identify the command sent]***************************************
RECEIVE_ADDRESS						;Nothing is done with the address
	bcf		STATUS,RP0
	bcf		STATUS,RP1	

	;To balance the time of I2C interruptions in motor control
	call	COUNT_WIDTH_R
	call	COUNT_WIDTH_L

	clrf	FLAGS_I2C	
	clrf	BUF_I2C	
	return				
;*********************************************************************************

;********** [Identify the command sent]***************************************
IDENTIFY_COMMAND

	rlf		BUF_I2C,W
	btfss	STATUS,C
	goto	COMMAND_MOVEMENT			;Two types og commands:Movement    MSB=0
	goto	COMMAND_INFORMATION			;					   Information MSB=1

COMMAND_MOVEMENT
	movfw	BUF_I2C						;The data of the comand is extracted:
	clrc								
	rrf		BUF_I2C,F
	bcf		FLAGS_COM,LEFT_RIGHT		;Left or Right
	btfsc	STATUS,C
	bsf		FLAGS_COM,LEFT_RIGHT
	
	clrc	
	rrf		BUF_I2C,W
	bcf		FLAGS_COM,TYPE_COM			;Type of command: time or distance
	btfsc	STATUS,C
	bsf		FLAGS_COM,TYPE_COM
	
	movwf	COMMAND	
	
	movfw	COMMAND						;It is verified that the movement command has been sent
	btfsc	STATUS,Z		
	bsf		FLAGS_I2C,COMMAND_REC

	movfw	COMMAND
	sublw	CHANGE_SPEED				;Command change speed of one of the engines
	btfsc	STATUS,Z
	bsf		FLAGS_I2C,COMMAND_REC

	movfw	COMMAND
	sublw	ROTATE						;Command rotate on a wheel
	btfsc	STATUS,Z
	bsf		FLAGS_I2C,COMMAND_REC

	movfw	COMMAND
	sublw	CURVE						;COMMAND CURVE
	btfsc	STATUS,Z
	bsf		FLAGS_I2C,COMMAND_REC

	movfw	COMMAND
	sublw	STOP_MICROBOT				;COMMAND STOP engines
	btfsc	STATUS,Z
	goto	ACC_STOP_MICROBOT
	
	;***DEBUG
	movfw	COMMAND
	sublw	TUNE						;Command to autoconfigure the KRCs
	btfsc	STATUS,Z
	goto	ACC_TUNE

	btfss	FLAGS_I2C,COMMAND_REC
	goto	INCORRECT_COMMAND		    ;Command not recognized
	return							

COMMAND_INFORMATION
	movfw	BUF_I2C						;An information command has been sent
	movwf	COMMAND_INFO		

	movfw	COMMAND_INFO
	sublw	RESET_DIST
	btfsc	STATUS,Z
	goto	ACC_RESET_DIST				;Command reset the distance traveled

	movfw	COMMAND_INFO
	sublw	RESET_TIME
	btfsc	STATUS,Z
	goto	ACC_RESET_TIME				;Command to reset the time elapsed since
										;the last movement command
	movfw	COMMAND_INFO
	sublw	RESET_KILO
	btfsc	STATUS,Z
	goto	ACC_RESET_KILO				;Command to reset the odometer.

	movfw	COMMAND_INFO				;Time is requested
	sublw	STATUS_T					;The next command I2C will be read
	btfsc	STATUS,Z
	return

	movfw	COMMAND_INFO				;Angle is requested
	sublw	STATUS_A					;The next command I2C will be read
	btfsc	STATUS,Z
	return

	movfw	COMMAND_INFO				;Right distance is requested:
	sublw	STATUS_D_R					;The next command I2C will be read
	btfsc	STATUS,Z
	return

	movfw	COMMAND_INFO				;Left distance is requested:
	sublw	STATUS_D_L					;The next command I2C will be read
	btfsc	STATUS,Z
	return

	movfw	COMMAND_INFO				;Right speed is requested:
	sublw	STATUS_V_R					;The next command I2C will be read
	btfsc	STATUS,Z
	return

	movfw	COMMAND_INFO				;Left speed is requested:
	sublw	STATUS_V_L					;The next command I2C will be read
	btfsc	STATUS,Z
	return

	movfw	COMMAND_INFO				;The state of the odometer is requested:
	sublw	STATUS_KILO					;The next command I2C will be read
	btfsc	STATUS,Z
	return

	goto	INCORRECT_COMMAND
;*********************************************************************************	
INCORRECT_COMMAND						;The sent command is not recognized
	bcf		STATUS,RP0
	bcf		STATUS,RP1	
	bsf		FLAGS_I2C,COM_DATA_INVAL
	clrf	BUF_I2C
	clrf	COMMAND
	clrf	COMMAND_INFO
	call	STOP_COM
	return
;*****************************[Stop the Microbot]*********************************
ACC_STOP_MICROBOT						;Command to stop microbot
	clrf	BUF_I2C
	banksel	SSPSTAT
	bcf		SSPSTAT,BF
	bcf		STATUS,RP0
	bcf		STATUS,RP1
	call	STOP_COM					;It's going to STOP
	return
;***********************************************************************************
;************************[Configurate the Microbot]*********************************
ACC_TUNE								;Command to stop auto-configuring KRCs
	clrf	BUF_I2C
	banksel	SSPSTAT
	bcf		SSPSTAT,BF
	bcf		STATUS,RP0
	bcf		STATUS,RP1
	call	TUNE_COM
	return
;***********************************************************************************

ACC_RESET_DIST
	clrf	DIST_AUX_L
	clrf	DIST_AUX_R
	return

ACC_RESET_TIME
	clrf	CONT_TIME_L
	clrf	CONT_TIME_H
	return

ACC_RESET_KILO

	clrf	AVERAGE_KILO_H_TOTAL 
	clrf	AVERAGE_KILO_L_TOTAL  
	clrf	AVERAGE_KILO_H 
	clrf	AVERAGE_KILO_L
	clrf	KILO_L_L
	clrf	KILO_L_H
	clrf	KILO_R_L
	clrf	KILO_R_H
	return

;******************[The datas sent are collected]**********************************
RECEIVE_DATA
	bcf		PORTC,FINISHED				;Necessary due to the microbot doesn't detect this signal
										;if it is sent when everything is over.
	movfw	COMMAND
										;According to the command sent
	movfw	COMMAND
	btfsc	STATUS,Z		
	goto	RECEIVE_STRAIGHT_DATA_1

	movfw	COMMAND
	sublw	CHANGE_SPEED
	btfsc	STATUS,Z
	goto	RECEIVE_CHANGE_SPEED_DATA_1

	movfw	COMMAND
	sublw	ROTATE
	btfsc	STATUS,Z
	goto	RECEIVE_ROTATE_DATA_1

	movfw	COMMAND
	sublw	CURVE
	btfsc	STATUS,Z
	goto	RECEIVE_CURVE_DATA_1

	goto	INCORRECT_COMMAND
	
;***********************************************************************************

;***********************[STRAIGHT data is collected]********************************
RECEIVE_STRAIGHT_DATA_1

	
	btfsc	FLAGS_I2C,SPEED_REC
	goto 	RECEIVE_STRAIGHT_DATA_2
	bcf		STATUS,C
	rrf		BUF_I2C,W						;The sense comes with the speed
	movwf	SPEED_AUX
	bcf		FLAGS_PWM,SENSE_R	
	bcf		FLAGS_PWM,SENSE_L	
	btfsc	STATUS,C						;To the two engines the same sense
	bsf		FLAGS_PWM,SENSE_R	
	btfsc	STATUS,C
	bsf		FLAGS_PWM,SENSE_L

	addlw   .155							;Check if the speed is between 0-100
 	addlw   .101
	btfss	STATUS,C
	goto	INCORRECT_COMMAND

	movfw	SPEED_AUX

	sublw	SPEED_MIN

	movwf	SPEED_L							;The same speed for the two engines
	movwf	SPEED_R

	bsf		FLAGS_COM,COM_DIST_R
	bsf		FLAGS_COM,COM_DIST_L
	btfsc	FLAGS_COM,TYPE_COM				;If the command is of distance, 
	goto	$+7								;count the distance in the two engines
	bcf		FLAGS_COM,COM_DIST_R
	bcf		FLAGS_COM,COM_DIST_L

	movfw	SPEED_L							;It checks if the speed is 0 and if it is command distance
	sublw	SPEED_MIN
	btfsc	STATUS,Z
	goto	INCORRECT_COMMAND				;In this case it is incorrect command

	bsf		FLAGS_I2C,SPEED_REC				;Speed has been received
	clrf	BUF_I2C
	return

RECEIVE_STRAIGHT_DATA_2
	
	movf 	BUF_I2C,W						;This data is time or distance
	movwf	TIME							;According to the type of command
	movwf	DISTANCE_L
	movwf	DISTANCE_R
	btfss	STATUS,Z						;If the distance is 0 the command is infinite time
	goto	$+3
	bsf		FLAGS_COM,COM_DIST_L			;The command is passed to infinite time
	bsf		FLAGS_COM,COM_DIST_R			;The command is passed to infinite time

	;The distance is divided by two because they are passed in 1mm
	;Check if it is 1
	sublw	.1
	btfsc	STATUS,Z
	goto	$+5
	;It is divided between two
	clrc	
	rrf		DISTANCE_L,F
	clrc
	rrf		DISTANCE_R,F
	
	;The minimum distance is limited to 1cm (problems with time for motor control and investment)
	;It is checked if the distances are less than or equal 5.
	movf	DISTANCE_R,W
	btfsc	STATUS,Z
	goto	$+7	
	sublw	DISTANCE_MIN
	btfss	STATUS,C
	goto	$+4	
	movlw	DISTANCE_MIN
	movwf	DISTANCE_L	
	movwf	DISTANCE_R	

	bsf		FLAGS_I2C,TIME_DIST_REC			;Time or distance received

	goto	DATA_RECEIVED
;***********************************************************************************


;*****************[CHANGE_SPEED data is requested]***************************
RECEIVE_CHANGE_SPEED_DATA_1
	btfsc	FLAGS_I2C,SPEED_REC
	goto 	RECEIVE_CHANGE_SPEED_DATA_2

	btfss	FLAGS_COM,LEFT_RIGHT
	goto	CHANGE_SPEED_L
	goto	CHANGE_SPEED_R

CHANGE_SPEED_L								;You have to change the left speed
	bcf		STATUS,C
	rrf		BUF_I2C,W						;The sense comes with the speed
	movwf	SPEED_AUX
	bcf		FLAGS_PWM,SENSE_L				
	btfsc	STATUS,C
	bsf		FLAGS_PWM,SENSE_L

	addlw   .155							;Check if the speed is between 0-100
 	addlw   .101
	btfss	STATUS,C
	goto	INCORRECT_COMMAND

	movfw	SPEED_AUX
	sublw	SPEED_MIN

	movwf	SPEED_L

	bsf		FLAGS_COM,COM_DIST_R
	bsf		FLAGS_COM,COM_DIST_L
	btfsc	FLAGS_COM,TYPE_COM				;In this command if it is of distance
	goto	$+7								;will take into account only the motor on the left
	bsf		FLAGS_COM,COM_DIST_R			;Left is distance command but right not
	bcf		FLAGS_COM,COM_DIST_L

	movfw	SPEED_L							;It checks if the speed is 0 and if it is command distance
	sublw	SPEED_MIN
	btfsc	STATUS,Z
	goto	INCORRECT_COMMAND

	;Step counter variables

	bsf		FLAGS_I2C,SPEED_REC				;Speed received
	clrf	BUF_I2C
	return

CHANGE_SPEED_R
	bcf		STATUS,C
	rrf		BUF_I2C,W						;The sense comes with the speed
	movwf	SPEED_AUX
	bcf		FLAGS_PWM,SENSE_R		
	btfsc	STATUS,C
	bsf		FLAGS_PWM,SENSE_R

	addlw   .155							;Check if the speed is between 0-100
 	addlw   .101
	btfss	STATUS,C
	goto	INCORRECT_COMMAND

	movfw	SPEED_AUX
	sublw	SPEED_MIN

	movwf	SPEED_R						

	bsf		FLAGS_COM,COM_DIST_R
	bsf		FLAGS_COM,COM_DIST_L
	btfsc	FLAGS_COM,TYPE_COM				;In this command if it is of distance
	goto	$+7								;will take into account only the motor on the left
	bcf		FLAGS_COM,COM_DIST_R			;Left is distance command but right not
	bsf		FLAGS_COM,COM_DIST_L

	movfw	SPEED_R							;It checks if the speed is 0 and if it is command distance
	sublw	SPEED_MIN
	btfsc	STATUS,Z
	goto	INCORRECT_COMMAND				;In this case it is incorrect command

	;Step counter variables

	bsf		FLAGS_I2C,SPEED_REC				;Speed received
	clrf	BUF_I2C
	return

RECEIVE_CHANGE_SPEED_DATA_2

	movf 	BUF_I2C,W						;Time or distance received
	movwf	TIME
	movwf	DISTANCE_L
	movwf	DISTANCE_R
	btfss	STATUS,Z						;If the distance is 0 the command is infinite time
	goto	$+3
	bsf		FLAGS_COM,COM_DIST_L			;The command is passed to infinite time
	bsf		FLAGS_COM,COM_DIST_R			;The command is passed to infinite time

	;The distance is divided by two because they are passed in 1mm
	;Check if it is 1
	sublw	.1
	btfsc	STATUS,Z
	goto	$+5
	;Se divide entre dos
	clrc	
	rrf		DISTANCE_L,F
	clrc
	rrf		DISTANCE_R,F

	;The minimum distance is limited to 1cm (problems with time for motor control and investment)
	;It is checked if the distances are less than or equal 5.
	movf	DISTANCE_R,W
	btfsc	STATUS,Z
	goto	$+7	
	sublw	DISTANCE_MIN
	btfss	STATUS,C
	goto	$+4	
	movlw	DISTANCE_MIN
	movwf	DISTANCE_L	
	movwf	DISTANCE_R	


	bsf		FLAGS_I2C,TIME_DIST_REC			;Time or distance received
	goto	DATA_RECEIVED
;***********************************************************************************

;********************[ROTATE_WHEEL data is requested]*******************************
RECEIVE_ROTATE_DATA_1
	btfsc	FLAGS_I2C,SPEED_REC
	goto	RECEIVE_ROTATE_DATA_2

	bcf		FLAGS_COM,STOP_ANG				;Reset calculation of angle.

	bcf		STATUS,C	
	rrf		BUF_I2C,W						;The sense comes in the speed
	movwf	SPEED_AUX

	bcf		STATUS,C	
	rrf		BUF_I2C,W
	bsf		FLAGS_COM,COM_SEN
	btfss	STATUS,C
	bcf		FLAGS_COM,COM_SEN

	bsf		FLAGS_PWM,SENSE_L				
	bcf		FLAGS_PWM,SENSE_R
	btfss	FLAGS_COM,LEFT_RIGHT
	bcf		FLAGS_PWM,SENSE_L
	btfss	FLAGS_COM,LEFT_RIGHT
	bsf		FLAGS_PWM,SENSE_R

	addlw   .155							;Check if the speed is between 0-100
 	addlw   .101
	btfss	STATUS,C
	goto	INCORRECT_COMMAND

	movfw	SPEED_AUX
	sublw	SPEED_MIN

	movwf	SPEED_L
	movwf	SPEED_R

	
	bsf		FLAGS_COM,COM_DIST_R
	bsf		FLAGS_COM,COM_DIST_L
	btfsc	FLAGS_COM,TYPE_COM				;In this command if it is of distance 
	goto	$+7								;count the distance in the two engines
	bcf		FLAGS_COM,COM_DIST_R
	bcf		FLAGS_COM,COM_DIST_L

	movfw	SPEED_L							;It checks if the speed is 0 and if it is command distance
	sublw	SPEED_MIN
	btfsc	STATUS,Z
	goto	INCORRECT_COMMAND				;In this case it is incorrect command

	bsf		FLAGS_I2C,SPEED_REC
	clrf	BUF_I2C
	return
RECEIVE_ROTATE_DATA_2
	btfsc	FLAGS_I2C,TIME_DIST_REC
	goto 	RECEIVE_ROTATE_DATA_3
	
	movfw	BUF_I2C
	movwf	TYPE_ROTATE
	btfsc	TYPE_ROTATE,0
	goto	FOLLOW_ROTATE_DATA_2

	movlw	SPEED_MIN						;The minimum speed is set to the wheel
	btfss	FLAGS_COM,LEFT_RIGHT			;Stop: speed 0
	movwf	SPEED_L
	btfsc	FLAGS_COM,LEFT_RIGHT
	movwf	SPEED_R

	bcf		FLAGS_PWM,SENSE_R	
	bcf		FLAGS_PWM,SENSE_L
	btfsc	FLAGS_COM,COM_SEN
	bsf		FLAGS_PWM,SENSE_R	
	btfsc	FLAGS_COM,COM_SEN
	bsf		FLAGS_PWM,SENSE_L

FOLLOW_ROTATE_DATA_2
	bsf		FLAGS_I2C,TIME_DIST_REC
	return

RECEIVE_ROTATE_DATA_3

	movfw	BUF_I2C

	btfsc	FLAGS_COM,TYPE_COM
	goto	$+5
	addlw   .155							;Check if the angle is between 0-100
 	addlw   .101
	btfss	STATUS,C
	goto	INCORRECT_COMMAND

	movf	BUF_I2C,W
	movwf	TIME

	btfss	STATUS,Z
	goto	$+3
	bsf		FLAGS_COM,COM_DIST_R
	bsf		FLAGS_COM,COM_DIST_L


	;The minimum rotation is limited to 14.4º (problems with time for control and investment of motors)
	;It is checked if the distances are less than or equal 5.
	movf	BUF_I2C,W
	btfsc	STATUS,Z
	goto	$+6	
	sublw	ROT_MIN 
	btfss	STATUS,C
	goto	$+3	
	movlw	ROT_MIN 
	movwf	BUF_I2C
			
	btfss	TYPE_ROTATE,0
	call	CALCULATE_DIST_WHEEL			;It is calculated the distance to travel through the
	btfsc	TYPE_ROTATE,0
	call	CALCULATE_DIST_CENTER			;It is calculated the distance to travel through the
	
	movwf	DISTANCE_L						;wheels through the angle (0-100)
	movwf	DISTANCE_R	

	;Step counter variables

	goto	DATA_RECEIVED


;************************************************************************************



;***************************[CURVE data is requested]********************************
RECEIVE_CURVE_DATA_1
	btfsc	FLAGS_I2C,SPEED_R_REC
	goto 	RECEIVE_CURVE_DATA_2

	bcf		STATUS,C
	rrf		BUF_I2C,W						;The sense comes with the speed
	movwf	SPEED_AUX
	bcf		FLAGS_PWM,SENSE_R	
	bcf		FLAGS_PWM,SENSE_L	
	btfsc	STATUS,C
	bsf		FLAGS_PWM,SENSE_R	
	btfsc	STATUS,C
	bsf		FLAGS_PWM,SENSE_L

	addlw   .155							;Check if the speed is between 0-100
 	addlw   .101
	btfss	STATUS,C
	goto	INCORRECT_COMMAND

	movfw	SPEED_AUX

	movwf	SPEED_L	
	movwf	SPEED_R
	bsf		FLAGS_I2C,SPEED_R_REC
	clrf	BUF_I2C
	return
RECEIVE_CURVE_DATA_2
	btfsc	FLAGS_I2C,TIME_DIST_REC
	goto 	RECEIVE_CURVE_DATA_3

	btfsc	FLAGS_COM,LEFT_RIGHT			;The second data is the radio
	comf	BUF_I2C,F						;According to where you want to give the curve,
											;this value will be added and subtracted 
	btfsc	FLAGS_COM,LEFT_RIGHT			;at two speeds.
	incf	BUF_I2C,F

	btfss	FLAGS_PWM,SENSE_L
	decf	BUF_I2C,F

	btfss	FLAGS_PWM,SENSE_L				
	comf	BUF_I2C,F

	movfw 	BUF_I2C
	subwf	SPEED_L,W
	movwf	SPEED_AUX

	addlw   .155							;Check if the speed is between 0-100
 	addlw   .101
	btfss	STATUS,C
	goto	INCORRECT_COMMAND

	movfw	SPEED_AUX
	sublw	SPEED_MIN
	movwf	SPEED_L

	movfw	BUF_I2C
	addwf	SPEED_R,W
	movwf	SPEED_AUX

	addlw   .155							;Check if the speed is between 0-100
 	addlw   .101
	btfss	STATUS,C
	goto	INCORRECT_COMMAND

	movfw	SPEED_AUX
	sublw	SPEED_MIN
	movwf	SPEED_R
	
	movfw	SPEED_R							;Default value (bad result)
	movfw	SPEED_L

	bsf		FLAGS_COM,COM_DIST_R
	bsf		FLAGS_COM,COM_DIST_L
	btfsc	FLAGS_COM,TYPE_COM				;In this command if it is of distance 
	goto	$+3								;count the distance in the two engines
	bcf		FLAGS_COM,COM_DIST_R
	bcf		FLAGS_COM,COM_DIST_L

	bsf		FLAGS_I2C,TIME_DIST_REC
	return
RECEIVE_CURVE_DATA_3

	movf 	BUF_I2C,W						;Finally the time or the distance is received
	movwf	DISTANCE_L
	movwf	DISTANCE_R
	movwf	TIME
	btfss	STATUS,Z						;If the distance is 0 the command is infinite time
	goto	$+3
	bsf		FLAGS_COM,COM_DIST_L			;The command is passed to infinite time
	bsf		FLAGS_COM,COM_DIST_R			;The command is passed to infinite time


	;The distance is divided by two because they are passed in 1mm
	;Check if it is 1
	sublw	.1
	btfsc	STATUS,Z
	goto	$+5
	;It is divided between two
	clrc	
	rrf		DISTANCE_L,F
	clrc
	rrf		DISTANCE_R,F

	;The minimum distance is limited to 1cm (problems with time for motor control and investment)
	;It is checked if the distances are less than or equal 5.
	movf	DISTANCE_R,W
	btfsc	STATUS,Z
	goto	$+7	
	sublw	DISTANCE_MIN
	btfss	STATUS,C
	goto	$+4	
	movlw	DISTANCE_MIN
	movwf	DISTANCE_L	
	movwf	DISTANCE_R	


	goto 	DATA_RECEIVED	

;***********************************************************************************

;****************************[All data received]************************************
DATA_RECEIVED
	
	bsf		FLAGS_I2C,COM_DATA_INVAL
	call	MOVE
	return
;***********************************************************************************

;*****************[Switch where you choose which data to send]**********************
STATE_SW
	
	bcf		STATUS,RP0
	bcf		STATUS,RP1

	movfw	COMMAND_INFO				;According to the information command received
	sublw	STATUS_T					;the required information is sent
	btfsc	STATUS,Z
	goto	SEND_TIME

	movfw	COMMAND_INFO
	sublw	STATUS_A
	btfsc	STATUS,Z
	goto	SEND_ANGLE

	movfw	COMMAND_INFO
	sublw	STATUS_D_R
	btfsc	STATUS,Z
	goto	SEND_DIST_R

	movfw	COMMAND_INFO
	sublw	STATUS_D_L
	btfsc	STATUS,Z
	goto	SEND_DIST_L

	movfw	COMMAND_INFO
	sublw	STATUS_V_R
	btfsc	STATUS,Z
	goto	SEND_SPEED_R

	movfw	COMMAND_INFO
	sublw	STATUS_V_L
	btfsc	STATUS,Z
	goto	SEND_SPEED_L

	movfw	COMMAND_INFO
	sublw	STATUS_KILO
	btfsc	STATUS,Z
	goto	SEND_KILO
	movlw	0xFF
	goto	SEND_INFO_I2C

SEND_TIME								;The elapsed time is sent
	bsf		FLAGS_I2C,END_STATE
	clrf	COMMAND_INFO
	movfw	CONT_TIME_H
	goto	SEND_INFO_I2C
SEND_DIST_R								;The right distance passed is sent
	bsf		FLAGS_I2C,END_STATE			;since the last command
	clrf	COMMAND_INFO
	;It is divided between two to send the distance in 1mm reso
	clrc
	rlf		DIST_AUX_R,W
	goto	SEND_INFO_I2C
SEND_DIST_L								;The right distance passed is sent
	bsf		FLAGS_I2C,END_STATE			;since the last command
	clrf	COMMAND_INFO
	;It is divided between two to send the distance in 1mm reso
	clrc
	rlf		DIST_AUX_L,W
	clrf	COMMAND_INFO
	goto	SEND_INFO_I2C
SEND_SPEED_R
	bsf		FLAGS_I2C,END_STATE			;The right speed is sent
	clrf	COMMAND_INFO
	movfw	SPEED_R
	sublw	SPEED_MIN
	goto	SEND_INFO_I2C
SEND_SPEED_L					
	bsf		FLAGS_I2C,END_STATE			;The left speed is sent
	clrf	COMMAND_INFO
	movfw	SPEED_L
	sublw	SPEED_MIN
	goto	SEND_INFO_I2C

SEND_ANGLE

	bsf		FLAGS_I2C,END_STATE
	clrf	COMMAND_INFO

	movfw	COMMAND
	sublw	ROTATE
	btfss	STATUS,Z
	goto	$+4
	
	btfss	TYPE_ROTATE,0	
	goto	CAL_ANGLE_WHEEL				;Through the distance traveled,
	goto	CAL_ANGLE_CENTER			;Through the distance traveled,
										;calculate the angle						

	movlw	0xFF
	goto	SEND_INFO_I2C
;by the average of the two distances

SEND_KILO

	movfw	N							;The distance traveled is calculated
	btfss	STATUS,Z					;by the average of the two distances
	goto	SEND_KILO_2					;of the two engines

	clrc
	movfw   KILO_L_L              
   	addwf   KILO_R_L,W          
	movwf	AVERAGE_KILO_L

    movfw   KILO_L_H           
    skpnc
    incf    KILO_L_H,W
    addwf   KILO_R_H,W
	movwf	AVERAGE_KILO_H
	 
	btfss	AVERAGE_KILO_H,7
	goto	$+4
	comf	AVERAGE_KILO_H,F
	comf	AVERAGE_KILO_L,F
	incf	AVERAGE_KILO_L,F	


	incf	N,F
	movfw	AVERAGE_KILO_L;_TOTAL 
	goto	SEND_INFO_I2C
	   
SEND_KILO_2
	clrf	N
	clrf	COMMAND_INFO
	movfw	AVERAGE_KILO_H;_TOTAL

	bsf		FLAGS_I2C,END_STATE

	goto	SEND_INFO_I2C


SEND_INFO_I2C
	movwf	SSPBUF
	banksel SSPSTAT
	bsf		SSPSTAT,BF
	banksel	SSPCON
	bsf		SSPCON,CKP
	bcf		STATUS,RP0
	bcf		STATUS,RP1
	return	

FINAL_STATE
	bcf		FLAGS_I2C,END_STATE
	return	





