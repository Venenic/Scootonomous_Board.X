/*******************************************************************************
File:			loadCells.h
Authors:		Kyle Hedges
Date:			Jan 31, 2020
Last Modified:	Jan 31, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

This file handles the operation of the four load cells used to measure the 
center of gravity on the board's surface.

ADC clock:
//					-RB0 (Pin 33)
//					-PWM1 module
//					-500kHz, 50% DC
//					-Interrupt on negative change
//
//					Data pins:
//					-Load cells [3:0] use RD[4:7] (Pins 27-30) respectively
//
*******************************************************************************/
#ifndef LOADCELLS_H
#define	LOADCELLS_H

//PWM output pin (RB0) setup definitions
#define ADC_CLK_TRIS TRISBbits.TRISB0 
#define ADC_CLK_ANSEL ANSELBbits.ANSELB0 
#define RB0_SOURCE RB0PPS 
#define PWM1_P2_OUT 0x19 

//so initializeLoadCells: ------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description: 	Initializes the PWM clock used to access the Hx711 and the 
//					negative edge IOC used to read in the data. Additional pins 
//					are configured as inputs to recieve this data.
//
// Created by:		Kyle Hedges 
// Last Modified:	Jan 31, 2020
//------------------------------------------------------------------------------
void initializeLoadCells(void);



#endif	/* LOADCELLS_H */

