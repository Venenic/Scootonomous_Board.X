/*******************************************************************************
File:			serialOutput.h
Authors:		Kyle Hedges
Date:			Feb 1, 2020
Last Modified:	Feb 3, 2020
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

#include "serialOutput.h"
#include "hardwareDefinitions.h"
 
#define BUFFER_INDEX_MASK 127 //Buffer size is 128 (127+1))

volatile unsigned char txIndex;
unsigned char storeIndex;
volatile char outputBuffer[BUFFER_INDEX_MASK +1]; //If buffer is too small and it overlaps the output will break
volatile unsigned char queueLength;

//so serialOutput_ISR: ------------------------------------------------------
// Parameters:		low_priority: It is a low priority interrupt
//					irq(U1TX): Interrupt vector source is UART1 TX
//					base(8): Base address of the vector table (8 is default)
// Returns:			void 
//
// Description:		Detects that the transmission buffer is empty and sends a
//					character from a string.
//
//					Problem: Does not copy over the string, so it will only
//					print a string as long as it remains in memory.
//					Potential Fix: Make a buffer to hold the string and print 
//					characters from that buffer.
//------------------------------------------------------------------------------

void __interrupt(low_priority,irq(U1TX),base(8)) serialOutput_ISR()
{
	U1TXB = outputBuffer[txIndex];
	
    if(outputBuffer[txIndex] == '\0'){
        queueLength -= 1;
		if(queueLength == 0){
            //Peripheral Interrupt Enable Register 4
            PIE4bits.U1TXIE = DISABLE_INTERRUPT; // [1] Disable transmit interrutpt
        }      
    }
	

	txIndex = (txIndex+1)&BUFFER_INDEX_MASK; //buffer length of 8 max

	
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
	
	
    storeIndex = 0;
    txIndex = 0;
    queueLength = 0; 

	U1CON0bits.TXEN = 1; //[5] Enable to transmitter 
    U1CON1bits.ON = 1; // [7] //Enable Serial Port
}


//so sendString: ------------------------------------------------------
// Parameters:		message: A pointer to the start of a string
// Returns:			void
//
// Description:		Adds the pointer to the start of a string to the output
//					buffer
//------------------------------------------------------------------------------
void sendString(char *message)
{
	while(*message != '\0')
	{
		outputBuffer[storeIndex] = *message;
        message++;
        storeIndex = (storeIndex + 1) & BUFFER_INDEX_MASK; 
	}
    outputBuffer[storeIndex] = '\0';
    storeIndex = (storeIndex + 1) & BUFFER_INDEX_MASK; 
    
	queueLength += 1; //Needs to be before interrupt enable
	//Peripheral Interrupt Enable Register 4
	PIE4bits.U1TXIE = ENABLE_INTERRUPT; // [1] Enable UART 1 Transmits interrutpt enable
}
