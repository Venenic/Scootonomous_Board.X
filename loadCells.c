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
//					-500kHz, 50% DC
//					-Interrupt on negative change
//
//					Data pins:
//					-Cell [3:0] use RD[4:7] (Pins 27-30)
//
*******************************************************************************/

#include <xc.h>
#include "loadCells.h"
#include "hardwareDefinitions.h"

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
void initializeLoadCells(void)
{
	ADC_CLK_TRIS = OUTPUT_PIN;
	ADC_CLK_ANSEL = DIGITAL_INPUT_PIN;
	
	RB0_SOURCE = PWM1_P2_OUT; //RB0 configured as PWM1_P2 output
	
}

