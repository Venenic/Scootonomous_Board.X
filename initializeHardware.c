/****************************************************************
File:		initializeHardware.c
Authors:	Kyle Hedges
Date:		Jan 27, 2020
(c) 2020 Lakehead University

DEVICE:	PIC18F47Q43

This file initializes the PIC hardware
****************************************************************/

#include <xc.h>

#include "hardwareDefinitions.h"
#include "initializeHardware.h"
#include "initializeOscillator.h"
#include "loadCells.h"

#include "device_config.h"

void initializeInterrupts(void);
void initializeADC_CLK(void);
void initializeOscillator(void);
void initializeUART(void);

void initializeHardware(void)
{
	initializeOscillator();
    initializeLoadCells();
	enableADC_CLK();
    
    TRISBbits.TRISB4 = OUTPUT_PIN;
    
    RB4PPS = 0x20; //RB4 configured as TX1 output
    
	initializeUART();
    initializeInterrupts();
    
    
}

void initializeInterrupts(void)
{
    //Interrupt Control Register 0
    //External interrupt pins aren't used in this design
    INTCON0bits.GIEH = 0; // [7] Disable all high priority interrupts for setup
    INTCON0bits.GIEL = 0; // [6] Disable all low priority interrupts for setup
    INTCON0bits.IPEN = 1; // [5] Enable priority levels on interrupts
    INTCON0bits.INT2EDG = 1; //[2] Interrupt on rising edge of INT2 pin 
    INTCON0bits.INT1EDG = 1; //[1] Interrupt on rising edge of INT1 pin 
    INTCON0bits.INT0EDG = 1; //[0] Interrupt on rising edge of INT0 pin 
    
    //Interrupt Control Register 1
    //INTCON1 is read only and holds the status flags (Which is executing)
    
    //Interrupt Vector Table Base Address
    //IVTBASE is left at the default address of 8
    
    //Interrupt Vector Table Address
    //IVTAD is read only and stores the calculated interrupt vector address on an interrupt
    
    //Interrupt Vector Table Lock Register
    //Following sequence sets the lock bit so that IVTBASE can't be altered
    //Reset is 0, so registers can be written by default
    IVTLOCKbits.IVTLOCKED = 0x55;
    IVTLOCKbits.IVTLOCKED = 0xAA;
    IVTLOCKbits.IVTLOCKED = 0x01; // lock IVT
    
    //Shadow Control Register
    SHADCONbits.SHADLO = 0; //[0] Default state. Not really sure what it does
 
    
    INTCON0bits.GIEH = 1; // [7] Enable all high priority interrupts
    INTCON0bits.GIEL = 1; // [6] Enable all low priority interrupts
    
}

void initializeUART(void)
{
    //UART1 Control Register 0
    U1CON0bits.BRGS = 1; // [7] Baud Rate speed select used for baud rate calculation
    U1CON0bits.TXEN = 0; //[5] Disable to transmitter for setup
    U1CON0bits.RXEN = 0; // [4] Disable the receiver
    U1CON0bits.MODE = 0b0000; // [3:0] Set to Asynchronous 8-bit mode
    
    //UART1 Control Register 1
    U1CON1bits.ON = 0; // [7] //Serial port disabled for setup
    
    //UART Baud Rate Generator
    //Baud rate = Fosc*(1+(BRGS*3))/(16*(BRG-1))
    //          = 64MHz * (1 + 1*3)/(16*(1668-1))
    U1BRGHbits.BRGH = 6; // [15:8] High byte
    U1BRGLbits.BRGL = 132; // [7:0] Low byte
    
    U1CON0bits.TXEN = 1; //[5] Enable to transmitter 
     U1CON1bits.ON = 1; // [7] //Enable Serial Port
     
    PIE4bits.U1TXIE = 1; // [1] Enable UART 1 Transmits enable

}
