/*******************************************************************************
File:			initializeOscillator.c
Authors:		Kyle Hedges
Date:			Jan 31, 2020
Last Modified:	Jan 31, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

This file initializes the system oscillator with the following parameters:
-Internal high frequency oscillator
-Fosc = 64MHz

*******************************************************************************/

#include <xc.h>

//so initializeHardware: -------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description: 	Initializes the system oscillator to 64MHz using the 
//					internal oscillator (HFINTOSC)
//
// Created by:		Kyle Hedges 
// Last Modified:	Jan 31, 2020
//------------------------------------------------------------------------------
void initializeOscillator(void)
{
    //Active Clock Tuning Register
    ACTCONbits.ACTEN = 0; // [7] Tuning controlled by OSCTUNE;
    ACTCONbits.ACTUD = 0; // [6] OSTUNE software updates are allowed;
    ACTCONbits.ACTLOCK = 0; //[3] HFINTOSC not locked (frequency can drift)
    ACTCONbits.ACTORS = 0; // [1] Tuning within OSCTUNE range (High accuracy) 
    
    //Oscillator Control Register 1
    //Used along with OSCFRQ to set Fosc to 64MHz
    OSCCON1bits.NOSC = 0b110; //[6:4] HFINTOSC clock source
    OSCCON1bits.NDIV = 0b0000; //[3:0] Clock divider of 1
    
    //Oscillator Control Register 2
    //OSCCON2 is read only and displays current oscillator source and divider
    
    //Oscillator Control Register 3
    OSCCON3bits.CSWHOLD = 0; // [7] Will switch if NOSC is requested
    OSCCON3bits.SOSCPWR = 0; // [6] Secondary oscillator in Low-Power mode
    //OSCCON3bits.ORDY [4] Read only
    //OSCCON3bits.NOSCR [3] Read only
    
    //HFINTOSC Frequency Tuning Register
    OSCTUNEbits.TUN = 0b000000; //[5:0] Operates at center frequency
    
    //HFINTOSC Frequency Selection Register
    OSCFRQbits.HFFRQ = 0b1000; // [3:0] HFINTOSC is set to 64MHz
    
    //Oscillator Status Register
    //OSCSTAT is read only and displays read status of the oscillators
    
    //Oscillator Enable Register
	//Used to manually enable oscillators
    //By default, all oscillators are disabled
    OSCENbits.EXTOEN = 0; // [7] EXTOSC is enabled by peripheral request
    OSCENbits.HFOEN = 0; // [6] HFINTOSC is enabled by peripheral request
    OSCENbits.MFOEN = 0; // [5] MFOEN is enabled by peripheral request
    OSCENbits.LFOEN = 0; // [4] LFOEN is enabled by peripheral request
    OSCENbits.SOSCEN = 0; // [3] SOSCEN is enabled by peripheral request
    OSCENbits.ADOEN = 0; // [2] ADCRC is enabled by peripheral request
}
