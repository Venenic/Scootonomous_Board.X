/*******************************************************************************
File:			loadCells.h
Authors:		Kyle Hedges
Date:			Jan 31, 2020
Last Modified:	Mar. 14, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

This file handles the operation of the four load cells used to measure the 
center of gravity on the board's surface.

ADC clock:
					-RB0 (Pin 33)
					-PWM1 module
					-500kHz, 0.25us on, 1.75us off
					-Low priority intterupt at end of ton

					Data pins:
					-Load cells [3:0] use RD[4:7] (Pins 27-30) respectively
					
Board Layout (Top View):	1---F---3
							|		|
							2---B---4

*******************************************************************************/
#ifndef LOADCELLS_H
#define	LOADCELLS_H

#include <stdint.h>
#include <stdbool.h>

//PWM output pin (RB0) setup definitions
#define ADC_CLK_TRIS TRISBbits.TRISB4 
#define ADC_CLK_SLR SLRCONBbits.SLRB4
#define ADC_CLK_PPS RB4PPS 
#define PWM3_P1_OUT 0x1C //PWM3_P1 output source identifier

#define NUMBER_OF_PULSES 25
//25 -> Gain of 128
//27 -> Gain of 64

//Load cell IO pins 
#define LOAD_CELL_1_DATA_TRIS TRISAbits.TRISA3
#define LOAD_CELL_2_DATA_TRIS TRISAbits.TRISA2
#define LOAD_CELL_3_DATA_TRIS TRISDbits.TRISD2
#define LOAD_CELL_4_DATA_TRIS TRISDbits.TRISD3

#define LOAD_CELL_1_DATA_ANSEL ANSELAbits.ANSELA3
#define LOAD_CELL_2_DATA_ANSEL ANSELAbits.ANSELA2
#define LOAD_CELL_3_DATA_ANSEL ANSELDbits.ANSELD2
#define LOAD_CELL_4_DATA_ANSEL ANSELDbits.ANSELD3

#define LOAD_CELL_1_DATA_IN PORTAbits.RA3
#define LOAD_CELL_2_DATA_IN PORTAbits.RA2
#define LOAD_CELL_3_DATA_IN PORTDbits.RD2
#define LOAD_CELL_4_DATA_IN PORTDbits.RD3

#define NUMBER_OF_LOAD_CELLS 4

//Index definitions
#define LOAD_CELL_FL 0
#define LOAD_CELL_BL 1
#define LOAD_CELL_FR 2
#define LOAD_CELL_BR 3

#define LOAD_CELL_1_ZERO -6947
#define LOAD_CELL_2_ZERO -401121
#define LOAD_CELL_3_ZERO -68488
#define LOAD_CELL_4_ZERO 327445


//Will only hold 24 bits, but that's slower than using 32 for some reason
typedef struct loadCell {
	int32_t rawData;
} loadCell;
	

//so initializeLoadCells: ------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description: 	Initializes the PWM clock used to access the Hx711 and the 
//					negative edge IOC used to read in the data. Additional pins 
//					are configured as inputs to recieve this data.
//------------------------------------------------------------------------------
void initializeLoadCells(void);

//so enableADC_CLK: ------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description: 	Enables the PWM module
//------------------------------------------------------------------------------
void enableADC_CLK(void);

//so disableADC_CLK: ------------------------------------------------------
// Parameters:		void (Lol)
// Returns:			void 
//
// Description: 	Disables the PWM module
//------------------------------------------------------------------------------
void disableADC_CLK(void);

bool pollLoadCells(loadCell *);
bool isDataReady(void);
__int24 getData(void);

#endif	/* LOADCELLS_H */

