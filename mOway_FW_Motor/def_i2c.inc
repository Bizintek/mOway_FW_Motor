;********************************************
;*	Definitions, variables and constant  	*
;*	for 16F687 oriented for          		*
;*	control of motors and i2c (I2C)			*
;********************************************


;************************************
;*	   Definition of the pin        *
;************************************

;*	Port B
SDA				EQU	4		;I2C data signal (input)
SCL				EQU	6		;I2C clock signal (input)

;*	Port C
FINISHED		EQU	6		;Finished signal (output)


;************************************
;*	   Definition of commands		*
;************************************
STRAIGHT		EQU	0x00	;Command to go straight
CHANGE_SPEED	EQU	0x01	;Command to change the motor speed
ROTATE			EQU	0x02	;Command to rotate the microbot on its wheel
CURVE			EQU	0x03	;Command to curve
STOP_MICROBOT	EQU	0x04	;Command to stop the microbot
TUNE			EQU	0x05	;Commnad for auto tune KRC levels
DEL_EEPROM		EQU	0x06	;Commnad for deleting EEPROM data

STATUS_T		EQU	0x80	;Information command time
STATUS_A		EQU	0x81	;Informative command angle
STATUS_V_R		EQU	0x82	;Informative command speed right
STATUS_V_L		EQU	0x83	;Information command left speed
STATUS_D_R		EQU	0x84	;Information command distance right
STATUS_D_L		EQU	0x85	;Informative command distance left
STATUS_KILO		EQU	0x86	;Informative command kilometers account.

RESET_TIME		EQU	0xF0	;Reset the time
RESET_DIST		EQU	0xF1	;Reset the kilometers account
RESET_KILO		EQU	0xF2	;Reset the kilometers account

;************************************
;*	   Declaration of variables     *
;************************************

;*	Variables of the data
SPEED_L			EQU	0x20	;Speed received for the left motor
SPEED_R			EQU	0x21	;Speed received for the right motor
DISTANCE_L		EQU 0x22	;Distance for the left motor
DISTANCE_R		EQU 0x23	;Distance for the right motor
TIME			EQU	0x24	;Time received from the operation of the engines

;*	I2C Variables
FLAGS_I2C		EQU	0x25	;Flags for the control of I2C commands
FLAGS_COM		EQU	0x26	;Flags for the control for type of command (Distance or time)
BUF_I2C			EQU	0x27	; I2C Buffer
COMMAND			EQU 0x28	;Variable that keeps the current I2C movement command
COMMAND_INFO	EQU 0x29	;Variable that keeps the current I2C information command
TYPE_ROTATE		EQU 0x2A	;Variable that keeps the rotation speed
SPEED_AUX		EQU	0x2B	;Rotation angle
N				EQU	0x2C	;Index for the STATE command



BUF_I2C0		EQU	0x2D	;Variables for the calculation of angles
BUF_I2C1		EQU	0x2E	
TEMP0			EQU	0x2F
TEMP1			EQU	0x30

;*	Interruption Variables 
W_TEMP			EQU	0x71	;Variable to store the accumulator (W) in the interruptions. Common directions in all banks
STATUS_TEMP		EQU	0x72	;Variable to save the Status. Common directions in all banks

;* Autotuning Variabke


;************************************
;*	   Definition of the bits		*
;************************************

;*	Variable: FLAGS_I2C
COMMAND_REC		EQU 0		;To know if the command has been received
SPEED_REC		EQU 1		;Has received the speed
SPEED_L_REC		EQU	2		;Has received the left speed
SPEED_R_REC		EQU	3		;Has received the right speed
TIME_DIST_REC	EQU 4		;It has received the ignition time of the engines
END_STATE		EQU	5		;To indicate that you do not have to send any more data
COM_DATA_INVAL	EQU 6		;Invalid command or data

;*	Variable: FLAGS_COM
TYPE_COM		EQU 0		;Type of command '1':Time. '0':Distance
LEFT_RIGHT		EQU 1		;				 '1':Right.'0':Left
STATUS_VEL_ANG	EQU 2		;				 '1':Right.'0':Left
COM_DIST_R		EQU 3		;Right distance command
COM_DIST_L		EQU 4		;Left distance command
COM_SEN			EQU 5		;Sense of the command '1':FWD o '0':BACK
STOP_ANG		EQU 6		;Stop angle calculation. It has gone from 100.
		
		




