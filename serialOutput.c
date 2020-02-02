/*******************************************************************************
File:			serialOutput.h
Authors:		Kyle Hedges
Date:			Feb 1, 2020
Last Modified:	Feb 1, 2020
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

#include <xc.h>
#include <stdbool.h>

#include "serialOutput.h"
#include "hardwareDefinitions.h"
 
volatile bool senderBusy = false;
char *outputString = NULL;
//so serialOutput_ISR: ------------------------------------------------------
// Parameters:		low_priority: It is a low priority interrupt
//					irq(U1TX): Interrupt vector source is UART1 TX
//					base(8): Base address of the vector table (8 is default)
// Returns:			void 
//
// Description:		Detects that the transmission buffer is empty and sends the
//					next character in the string
//
// Created by:		Kyle Hedges 
// Last Modified:	Feb 1, 2020
//------------------------------------------------------------------------------

void __interrupt(low_priority,irq(U1TX),base(8)) serialOutput_ISR()
{
	U1TXB = *outputString;
	
    if(*outputString == '\0'){
		
		//Peripheral Interrupt Enable Register 4
		PIE4bits.U1TXIE = DISABLE_INTERRUPT; // [1] Disable transmit interrutpt
	
		senderBusy = false;
	}	
	else{
		outputString++;
	}
	
	//Peripheral Interrupt Request Register 4
	//PIRbits.U1TXIF is a read only bit and is cleared automatically
}


//so initializeLoadCells: ------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description: 	Initializes the serial output with the following settings:
//					--RB4 (Pin 37)
//					-UART1 module
//					-9600 Baud rate
//					-1 Stop bit
//					-8 Data bits
//					-No parity
//
// Created by:		Kyle Hedges 
// Last Modified:	Feb 1, 2020
//------------------------------------------------------------------------------
void initializeSerialOutput(void)
{
	//UART1 Control Register 1
    U1CON1bits.ON = 0; // [7] //Serial port disabled for setup
	
	SERIAL_OUTPUT_TRIS = OUTPUT_PIN;
	SERIAL_OUTPUT_PPS = UART1_TX_OUT;
	
	 //UART1 Control Register 0
    U1CON0bits.BRGS = 1; // [7] Baud Rate generator is high speed (4 clocks/bit)
    U1CON0bits.TXEN = 0; //[5] Disable to transmitter for setup
    U1CON0bits.RXEN = 0; // [4] Disable the receiver
    U1CON0bits.MODE = 0b0000; // [3:0] Set to Asynchronous 8-bit mode
	
	//UART Baud Rate Generator
    //Baud rate = Fosc*(1+(BRGS*3))/(16*(BRG-1))
    //          = 64MHz * (1 + 1*3)/(16*(1668-1))
    U1BRGHbits.BRGH = 6; // [15:8] High byte
    U1BRGLbits.BRGL = 132; // [7:0] Low byte
	
	//Peripheral Interrupt Priority Register 4
	IPR4bits.U1TXIP = LOW_PRIORITY; // [3] Transmit interrupt is low priority
    
	//Peripheral Interrupt Enable Register 4
	PIE4bits.U1TXIE = DISABLE_INTERRUPT; // [1] Disable UART 1 Transmits interrutpt enable
	
	U1CON0bits.TXEN = 1; //[5] Enable to transmitter 
    U1CON1bits.ON = 1; // [7] //Enable Serial Port
}


//so sendString: ------------------------------------------------------
// Parameters:		message: A pointer to the start of the string being sent
// Returns:			bool: 	true if the string is being sent
//							false if busy sending a previous string
//
// Description:		If the UART is not busy it copies the string over and
//					initiates the transmission process.
//					IMPORTANT: Strings must be null terminated
//
// Created by:		Kyle Hedges 
// Last Modified:	Feb 1, 2020
//------------------------------------------------------------------------------
bool sendString(char *message)
{
	if(senderBusy) return false;
	else 
	{
		outputString = message;
		
		senderBusy = true;
		
		//Peripheral Interrupt Enable Register 4
		PIE4bits.U1TXIE = ENABLE_INTERRUPT; // [1] Enable UART 1 Transmits interrutpt enable
	}
	
	return true;
}
