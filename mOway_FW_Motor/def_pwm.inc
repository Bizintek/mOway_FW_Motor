;********************************************
;*	Definitions, variables and constants	*
;*	for 16F687 oriented for		            *
;*	control of motors and i2c (PWM).		*
;********************************************


;************************************
;*	   Definition of the pin    	*
;************************************

;*	Port A

MOTOR_L_A		EQU	2		;Signal A of the left motor (output)
ENCODER_L_EN	EQU	4		;Signal of activation of the left encoder (output)
MOTOR_L_B		EQU	5		;Signal B of the left motor (output)


;*	Port B
ENCODER_R_EN	EQU	5		;Right encoder activation signal (output)

;*	Port C
ENCODER_R		EQU	0		;Right encoder reading signal (input)
MOTOR_R_A		EQU	1		;Signal A of the right motor (output)

MOTOR_R_B		EQU	2		;Signal B of the right motor (output)
ENCODER_L		EQU	4		;Signal reading of the left encoder (input)



;************************************
;*	 Definition of constants		*
;************************************

SPEED_MIN		EQU .230		;Minimum speed of the uBot (maximum sum of the encoder counter)

SPEED_MAX_PWM	EQU .50			;Minimum speed of the uBot (maximum sum of the encoder counter)
SPEED_MIN_PWM	EQU .220		;Minimum speed of the uBot (maximum sum of the encoder counter)

CONST_CONT	EQU .4			;Constant for the pulse counter of the encoder.
THIGH_R_MIN	EQU .30			;Constant for the pulse counter of the encoder.
THIGH_L_MIN	EQU .30			;Constant for the pulse counter of the encoder.

DISTANCE_MIN EQU .5		;Minimum distance to travel (number of flanks)
ROT_MIN EQU .4				;Minimum distance to travel (number of flanks)

;************************************
;*	Declaration of variables		*
;************************************

;*	Variables of the encoders


CON_STATUS  EQU 0x33		;Variable that controls the flow of the main CONTROL program

DIST_AUX_R	EQU 0x34		;Variable where the right distance of the current command is saved
DIST_AUX_L	EQU	0x35		;Variable where the left distance of the current command is saved

CONT_TIME_H EQU 0x36		;Variable where the current command time is saved (100 ms step)
CONT_TIME_L EQU 0x37		;Variable where the current command time is saved (0.5 ms step)

;DIFF_L 	EQU 0x38		;Difference of the width of the previous and left current encoder
;DIFF_R	 	EQU 0x39		;Difference of the width of the previous and right current encoder

STOP_L	EQU 0x3A		;Counter to check if the left wheel is stopped
STOP_R 	EQU 0x3B		;Counter to check if the right wheel is stopped

STEPS_L		EQU	0x3C		;Number of color changes in the left encoder
STEPS_R		EQU	0x3D		;Number of color changes in the right encoder

CONT_L_H	EQU	0x3E		;High register of the left motor counter.
CONT_L_L	EQU	0x3F		

CONT_R_H	EQU	0x40		;High register of the right motor counter.
CONT_R_L	EQU	0x41		

;*	Variables of the PWM
THIGH_L			EQU	0x50	;Width of the left positive pulse (value of a timer)
THIGH_R			EQU	0x51	;Width of the right positive pulse (value of a timer)

FLAGS_PWM		EQU	0x52	;Flags for PWM control

;*	Delays variables

CON_ACT		EQU	0x53	;Variable to control the flow of ACT

;*	Odometer variables

KILO_L_H		EQU	0x54	;High variable of the left odometer
KILO_L_L		EQU	0x55	;Low variable of the left odometer
KILO_R_H		EQU	0x56	;High variable of the right odometer
KILO_R_L		EQU 0x57	;Low variable of the right odometer
AVERAGE_KILO_L	EQU	0x58	;Low variable of the average distance traveled by uBot
AVERAGE_KILO_H	EQU	0x59	;High variable of the average distance covered by uBot
AVERAGE_KILO_L_TOTAL 	EQU	0x5A	;Low variable of the average distance traveled by total uBot
AVERAGE_KILO_H_TOTAL	EQU	0x5B	;High variable of the average distance traveled by total uBot


DELAY_PWM		EQU	0x31	;Delay the PWM so that the two INTs put the PWM at the same time

;************************************
;*	Definition of the bits		   *
;************************************

;*	Variable: FLAGS_PWM
SENSE_L	EQU	0			;Sense of the left motor (forward / backward)
SENSE_R	EQU	1			;Sense of the right motor (forward / backward)
SPEED_0_L		EQU	2			;Sense of the left motor (forward / backward)
SPEED_0_R		EQU	3			;Sense of the right motor (forward / backward)
KILO_L		EQU 4			;Indicates whether to add or subtract the left odometer 
KILO_R		EQU	5			;Indicates whether to add or subtract the right odometer 
PULSE_L		EQU	6			;Last left pulse performed (positive / negative)
PULSE_R		EQU	7			;Last right pulse performed (positive / negative)

;*	Variable: CON_STATUS: Variable that controls the flow of the main CONTROL program
ENC_L_STATUS	EQU	0		;Left Encoder in white or black
ENC_R_STATUS	EQU	1		;Right Encoder in white or black
CONT_L_START    EQU 2		;Start counter left when I have a positive edge on the encoder
CONT_R_START    EQU 3		;Start counter right when I have a positive edge on the encoder
ACT_L			EQU 4		;Act L when it stops counting the pulse of the left encoder
ACT_R			EQU 5		;Act R when it stops counting the pulse of the right encoder

;*  Variable: CON_ACT: Variable to control the flow of ACT
MEDIA_L			EQU 0		;Calculate left average
MEDIA_R			EQU 1		;Calculate right average
ACTUO_L			EQU 2		;Act on the left PWM
ACTUO_R			EQU 3		;Act on the right PWM
NO_PASO_L		EQU 4		;Don't count step because I come from overflowing the left counter
NO_PASO_R		EQU 5		;Don't count step because I come from overflowing the right counter
