/****************************************************************
File:		initializeHardware.h
Authors:	Kyle Hedges
Date:		Jan 27, 2020
(c) 2020 Lakehead
****************************************************************/

#ifndef INITIALIZEHARDWARE_H
#define	INITIALIZEHARDWARE_H

#define ENABLE_PB_TRIS TRISAbits.TRISA0
#define ENABLE_PB_ANSEL ANSELAbits.ANSELA0
#define ENABLE_PB_IN PORTAbits.RA0

#define ENABLE_PB_PRESSED 1 
#define ENABLE_PB_RELEASED 0

#define DEBUG_PIN_TRIS TRISAbits.TRISA1
#define DEBUG_PIN_OUT LATAbits.LATA1
#define DEBUG_PIN_SLRCON SLRCONAbits.SLRA1

#define STATUS_LED_R_TRIS TRISEbits.TRISE0
#define STATUS_LED_R_OUT LATEbits.LATE0

#define STATUS_LED_Y_TRIS TRISEbits.TRISE1
#define STATUS_LED_Y_OUT LATEbits.LATE1

#define STATUS_LED_G_TRIS TRISEbits.TRISE2
#define STATUS_LED_G_OUT LATEbits.LATE2

#define STATUS_LED_OFF 1
#define STATUS_LED_ON 0

void initializeHardware(void);

#endif	/* INITIALIZEHARDWARE_H */

