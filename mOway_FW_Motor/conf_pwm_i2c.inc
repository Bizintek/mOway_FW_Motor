;************************************
;*	Configuration of 16F687 for 	*
;*	motor control and i2c    		*
;************************************
 errorlevel -207
;*	Initial zeroing
	clrf	PORTA
	clrf	PORTB
	clrf	PORTC


;*	Configuration of the I2C
	banksel SSPSTAT
	clrf 	SSPSTAT
	banksel	PIR1
	bcf 	PIR1,SSPIF
	banksel	SSPADD
	movlw	0x10
	movwf	SSPADD
	banksel	SSPCON
	movlw	0x36
	movwf	SSPCON	


;*	Oscillator configuration
	banksel	OSCCON
	movlw	b'01110111'			;Oscillator to 8Mhz. Internal clock
	movwf	OSCCON

;	banksel	OSCTUNE
	movlw	b'00000000'			;Put the oscillator at 8Mhz
	movwf	OSCTUNE

;*	Oscillator stabilization
	;banksel	OSCCON
	STABILIZE_OSC				;Wait until the oscillator reaches the desired value
	clrwdt				
	btfss	OSCCON,HTS
	goto	STABILIZE_OSC

;*	Configuration of the TMR0
;	banksel	OPTION_REG
	movlw	b'00000010'			;The prescaler for the TMR0 is placed in 1: 8
	movwf	OPTION_REG
	
;*	Configuration of interruptions
	banksel	INTCON
	movlw	b'11000000'			;Enabled general interrupts and non-maskable interrupts
	movwf	INTCON


	banksel	PIE2
	movlw	b'00000000'			;No more interruptions
	movwf	PIE2

	banksel	IOCA
	movlw	b'00000000'			;No more interruption for change in port A
	movwf	IOCA

	banksel	IOCB
	movlw	b'00000000'			;No more interruption for change in port B
	movwf	IOCB

;*	CAD configuration
	banksel	ANSEL
	movlw	b'10010000'			;Analog input encoder
	movwf	ANSEL

	banksel	ANSELH
	movlw	b'00000000'			;No analog input
	movwf	ANSELH

;*	Configuration of pull-ups
	banksel	WPUA
	movlw	b'00110111'			;Pull-ups are enabled in port A
	movwf	WPUA

	banksel	WPUB
	movlw	b'00000000'			;Pull-ups are enabled in port B
	movwf	WPUB

;*	Configuration of the TMR1
	banksel	T1CON
	movlw	b'00110001'			;TMR1 siempre activo. Preescaler 1:8 (mismo q TMR0)
	movwf	T1CON


	;Configuration baudios
	banksel	SPBRG
    movlw 	.16               ;9600 baudios
    movwf 	SPBRG
	banksel	SPBRGH
    movlw 	.0            	  ;9600 baudios
    movwf 	SPBRGH
	banksel	TXSTA
    movlw 	b'00100100'       ; brgh = high (2)
    movwf 	TXSTA             ; enable Async Transmission, set brgh

	banksel	BAUDCTL
	bsf		BAUDCTL,BRG16

	banksel	RCSTA
    movlw 	b'00000000'       ; disable Async Reception
    movwf 	RCSTA 


;*	Configuración de E/S
	banksel	TRISA
	movlw	b'00000000'
	movwf	TRISA

;	banksel	TRISB
	movlw	b'01010000'
	movwf	TRISB

;	banksel	TRISC
	movlw	b'00011001'
	movwf	TRISC

	banksel PIE1
	clrf	PIE1
	bsf		PIE1,SSPIE

	banksel	PIR1
	clrf	PIR1

;	banksel	PIR2
	clrf	PIR2

;	banksel	INTCON
	bcf		INTCON,T0IF

;	banksel	PIR1
	bcf		PIR1,TMR1IF

