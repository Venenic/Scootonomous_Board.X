/*******************************************************************************
File:			serialOutput.h
Authors:		Kyle Hedges
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

Description: This file is responsible for transmitting serial data. 

Serial Settings:
				-RB4 (Pin 37)
				-UART1 module
				-9600 Baud rate
				-1 Stop bit
				-8 Data bits
				-No parity
*******************************************************************************/  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <stdbool.h>

//Serial ouptu pin (RB4) setup definitions
#define SERIAL_OUTPUT_TRIS TRISBbits.TRISB4
#define SERIAL_OUTPUT_PPS RB4PPS

//so initializeLoadCells: ------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description: 	Initializes the serial output with the following settings:
//					-RB4 (Pin 37)
//					-UART5 module
//					-9600 Baud rate
//					-1 Stop bit
//					-8 Data bits
//					-No parity
//------------------------------------------------------------------------------
void initializeSerialOutput(void);

//so sendString: ------------------------------------------------------
// Parameters:		char *: A pointer to the start of the string being sent. Max
//							string size is 256 currently.
// Returns:			void
//
// Description:		Copies source string to the outputBuffer where it will be
//					transmitted asynchronously over UART5
//------------------------------------------------------------------------------
void sendString(char *);

#endif	/* XC_HEADER_TEMPLATE_H */

