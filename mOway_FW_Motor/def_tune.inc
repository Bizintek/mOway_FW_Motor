;************************************
;*	Variables to store the          *
;*	values of the KRCs in EEPROM    *
;************************************
; Constants
EE_KRC_MAX_GO_L		EQU	0x00
EE_KRC_MIN_GO_L		EQU	0x01
EE_KRC_GO_L			EQU	0x02	; Average value between maximum and minimum when moving
EE_KRC_MAX_BACK_L	EQU	0x03
EE_KRC_MIN_BACK_L	EQU	0x04
EE_KRC_BACK_L		EQU	0x05	; Average value between the maximum and minimum when backing up

EE_KRC_MAX_GO_R		EQU	0x06
EE_KRC_MIN_GO_R		EQU	0x07
EE_KRC_GO_R			EQU	0x08
EE_KRC_MAX_BACK_R	EQU	0x09
EE_KRC_MIN_BACK_R	EQU	0x0A
EE_KRC_BACK_R		EQU	0x0B

EE_KRC_L			EQU	0x0C
EE_KRC_R			EQU	0x0D

EE_KRC_SUP_L		EQU	0x0E
EE_KRC_INF_L		EQU	0x0F
EE_KRC_SUP_R		EQU	0x10
EE_KRC_INF_R		EQU	0x11

; Variables
EE_TEMP_ADD			EQU	0x42
EE_USER_ADD			EQU 0x43
EE_USER_DATA		EQU 0x44

KRC_MIN				EQU	0x45	; Save the minimum value of the KRC
KRC_MAX				EQU	0x46	; Save the maximum value of the KRC

TEMP_VALUE			EQU	0x47
TEMP_VALUE2			EQU	0x48

