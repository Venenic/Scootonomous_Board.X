/*******************************************************************************
File:			serialOutput.h
Authors:		Kyle Hedges
Date:			Feb 1, 2020
Last Modified:	Feb 2, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

This file is responsible for transmitting serial data

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
#define UART1_TX_OUT 0x20; //UART1_TX output source identifier

#define MAX_STRING_LENGTH 256

//so initializeLoadCells: ------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description: 	Initializes the serial output with the following settings:
//					-RB4 (Pin 37)
//					-UART1 module
//					-9600 Baud rate
//					-1 Stop bit
//					-8 Data bits
//					-No parity
//------------------------------------------------------------------------------
void initializeSerialOutput(void);

//so sendString: ------------------------------------------------------
// Parameters:		char *: A pointer to the start of the string being sent
// Returns:			bool: 	true if the string is being sent
//							false if busy sending a previous string
//
// Description:		If the UART is not busy it copies the string over and
//					initiates the transmission process.
//					IMPORTANT: Strings must be null terminated
//------------------------------------------------------------------------------
bool sendString(char *);

#endif	/* XC_HEADER_TEMPLATE_H */

