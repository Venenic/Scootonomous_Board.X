/*******************************************************************************
File:			serialOutput.c
Authors:		Kyle Hedges
Date Created:	Feb 1, 2020
Last Modified:	Mar. 3, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

Description: This file is responsible for transmitting serial data

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
 
 //If buffer is too small and it overlaps the output will break
 //If made any bigger, remember to adjust Index data types to int
#define BUFFER_INDEX_MASK 255 //Buffer size is 128 (127+1))

#define UART5_TX_OUT 0x2C; //UART5_TX output source identifier

//Serial ouptu pin (RB4) setup definitions
#define SERIAL_OUTPUT_TRIS TRISBbits.TRISB5
#define SERIAL_OUTPUT_PPS RB5PPS

char outputBuffer[BUFFER_INDEX_MASK +1]; 
volatile unsigned char queueLength; //Strings in buffer pending transmission

//so serialOutput_ISR: ------------------------------------------------------
// Parameters:		low_priority: It is a low priority interrupt
//					irq(U5TX): Interrupt vector source is UART5 TX
//					base(8): Base address of the vector table (8 is default)
// Returns:			void 
//
// Description:		Detects that the transmission buffer is empty and sends a
//					character from a string.
// Last Modified:	Feb 19, 2020
//------------------------------------------------------------------------------
void __interrupt(low_priority,irq(U5TX),base(8)) serialOutput_ISR()
{
	static unsigned char txIndex = 0;
	
	char currentChar = outputBuffer[txIndex];
	
    if(currentChar == '\0'){
        queueLength -= 1;
		if(queueLength == 0){
            //Peripheral Interrupt Enable Register 13
            PIE13bits.U5TXIE = DISABLE_INTERRUPT;// [1] Disable transmit interrutpt
        }      
    } else U5TXB = currentChar;

	txIndex = (txIndex+1)&BUFFER_INDEX_MASK; //buffer length of 8 max

	//Peripheral Interrupt Request Register 13
	//PIR13bits.U5TXIF is a read only bit and is cleared automatically
}//eo serialOutput_ISR----------------------------------------------------------


//so initializeLoadCells: ------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description: 	Initializes the serial output with the following settings:
//					--RB4 (Pin 37)
//					-UART5 module
//					-9600 Baud rate
//					-1 Stop bit
//					-8 Data bits
//					-No parity
// Last Modified:	Feb 19, 2020
//------------------------------------------------------------------------------
void initializeSerialOutput(void)
{
	//UART1 Control Register 1
    U5CON1bits.ON = 0; // [7] //Serial port disabled for setup
	
	SERIAL_OUTPUT_TRIS = OUTPUT_PIN;
	SERIAL_OUTPUT_PPS = UART5_TX_OUT;
	
	 //UART1 Control Register 0
    U5CON0bits.BRGS = 1; // [7] Baud Rate generator is high speed (4 clocks/bit)
    U5CON0bits.TXEN = 0; //[5] Disable to transmitter for setup
    U5CON0bits.RXEN = 0; // [4] Disable the receiver
    U5CON0bits.MODE = 0b0000; // [3:0] Set to Asynchronous 8-bit mode
	
	//UART Baud Rate Generator
    //Baud rate = Fosc*(1+(BRGS*3))/(16*(BRG-1))
    //          = 64MHz * (1 + 1*3)/(16*(1668-1))
    U5BRGHbits.BRGH = 6; // [15:8] High byte
    U5BRGLbits.BRGL = 132; // [7:0] Low byte
	
	//Peripheral Interrupt Priority Register 13
	IPR13bits.U5TXIP = LOW_PRIORITY; // [3] Transmit interrupt is low priority
    
	//Peripheral Interrupt Enable Register 4
	PIE13bits.U5TXIE = DISABLE_INTERRUPT;// [1] Disable UART5 Transmit interrupt
	
    queueLength = 0; 

	U5CON0bits.TXEN = 1; //[5] Enable transmitter 
    U5CON1bits.ON = 1; // [7] //Enable Serial Port
}//eo initializeSerialOutput----------------------------------------------------


//so sendString: ------------------------------------------------------
// Parameters:		message: A pointer to the start of a string
// Returns:			void
//
// Description:		Copies source string to the outputBuffer where it will be
//					transmitted asynchronously over UART5
// Last Modified:	Feb 19, 2020
//------------------------------------------------------------------------------
void sendString(char *message)
{
	static unsigned char storeIndex = 0;
	
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
	PIE13bits.U5TXIE = ENABLE_INTERRUPT;// [1] UART5 Transmit interrutpt enable
}//eo sendString----------------------------------------------------------------
