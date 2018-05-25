;***************Definitions*******************

; Windows of
; Straight PCB1
;**********************************************************************************************
;	UPGRADE:	The value of the analog reading of the encoder sensors has been changed
;	DATE:			25/07/2012
;	PROGRAMMER:	Samuel Merinor
;**********************************************************************************************


LIMIT_HIGH_L		EQU 0x4A
LIMIT_LOW_L			EQU 0x4B

LIMIT_HIGH_R		EQU 0x4C
LIMIT_LOW_R			EQU 0x4D

LIMIT_R				EQU 0x4E	;Right KRC average level in EEPROM
LIMIT_L				EQU 0x4F	;Right KRC average level in EEPROM



ANALOG_LEFT			EQU	0x5C	;Analog value of the left encoder 
ANALOG_RIGHT		EQU	0x5D	;Analog value of the right encoder
CONTROL_FLAGS		EQU	0x5E	;Flag for the control routine
								;Reset at the start of the command
CONTROL_FLAGS_2		EQU	0x5F	;Flag for the control routine
								;Reset at the start of the command
DELAY_00			EQU	0x60	;Delay vatiables
DELAY_01			EQU	0x61	;Delay vatiables
DELAY_02			EQU	0x62	;Delay vatiables


CRONO_L				EQU	0x63	;Counter pulse width left
CRONO_R				EQU	0x64	;Counter pulse width rihgt

DIFF_L				EQU	0x65	;Difference for control I
DIFF_R				EQU	0x66	;Difference for control D

INERTIA_DELAY		EQU	0x67	;Delay for inertia

;***DEBUG
TUN_DELAY1			EQU	0x68	;Delay to configure the KRC
TUN_DELAY2			EQU	0x69

;*	Variable: CONTROL_FLAGS	
FIRST_READ_L		EQU 0		;First reading has been made     1 
								;First reading not done	         0	
FIRST_READ_R		EQU 1		;First reading has been made     1 
								;First reading not done     	 0
HIGH_LOW_L			EQU 2		;0 it is down    1 it is up
HIGH_LOW_R			EQU 3		;0 it is down 1 it is up

FIRST_CRONO_L		EQU 4		;Indicates whether to reject the first data of the counter I
FIRST_CRONO_R		EQU 5		;Indicates whether to reject the first data of the counter D

STOP_INIT			EQU 6		;Flag indicating that movement has begun
MOVE_INIT			EQU 7		;Flag indicating that movement has begun

;*	Variable: CONTROL_FLAGS_2	
FLANK_HIGH_L_DET	EQU 0		;Detected rising flank on left encoder
FLANK_LOW_L_DET		EQU 1		;Detected flank lowered in left encoder
FLANK_HIGH_R_DET	EQU 2		;Detected rising flank on right encoder
FLANK_LOW_R_DET		EQU 3		;Detected flank lowered in right encoder

