/*******************************************************************************
File:			loadCells.h
Authors:		Kyle Hedges
Date:			Jan 31, 2020
Last Modified:	Feb 4, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

This file handles the operation of the four load cells used to measure the 
center of gravity on the board's surface.

ADC clock:
					-RB0 (Pin 33)
					-PWM1 module
					-500kHz, 50% DC
					-Low priority intterupt at end of ton

					Data pins:
					-Load cells [3:0] use RD[4:7] (Pins 27-30) respectively

*******************************************************************************/
#include <stdbool.h>

#ifndef LOADCELLS_H
#define	LOADCELLS_H

//PWM output pin (RB0) setup definitions
#define ADC_CLK_TRIS TRISBbits.TRISB0 
#define ADC_CLK_ANSEL ANSELBbits.ANSELB0 
#define ADC_CLK_SLR SLRCONBbits.SLRB0
#define ADC_CLK_PPS RB0PPS 
#define PWM1_P1_OUT 0x18 //PWM1_P2 output source identifier

#define LOADCELL_1_DATA_TRIS TRISDbits.TRISD4
#define LOADCELL_2_DATA_TRIS TRISDbits.TRISD5
#define LOADCELL_3_DATA_TRIS TRISDbits.TRISD6
#define LOADCELL_4_DATA_TRIS TRISDbits.TRISD7

#define LOADCELL_1_DATA_ANSEL ANSELDbits.ANSELD4
#define LOADCELL_2_DATA_ANSEL ANSELDbits.ANSELD5
#define LOADCELL_3_DATA_ANSEL ANSELDbits.ANSELD6
#define LOADCELL_4_DATA_ANSEL ANSELDbits.ANSELD7

#define LOADCELL_1_DATA_IN PORTDbits.RD4
#define LOADCELL_2_DATA_IN PORTDbits.RD5
#define LOADCELL_3_DATA_IN PORTDbits.RD6
#define LOADCELL_4_DATA_IN PORTDbits.RD7

#define DATA_PULSES 24
#define MAX_PULSES 25
//25 -> Gain of 128
//27 -> Gain of 64

//Will only hold 24 bits, but that's slower than using 32 for some reason
typedef struct loadCellData {
	unsigned long cellData1;
	unsigned long cellData2;
	unsigned long cellData3;
	unsigned long cellData4;
	unsigned char sampleTime; //Time since last sample in ms
} loadCellData;
	

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
// Parameters:		void
// Returns:			void 
//
// Description: 	Disables the PWM module
//------------------------------------------------------------------------------
void disableADC_CLK(void);

bool pollLoadCells(loadCellData*);
bool isDataReady(void);
__int24 getData(void);

#endif	/* LOADCELLS_H */

