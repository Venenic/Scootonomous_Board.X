/*******************************************************************************
File:			initializeSysTick.h
Authors:		Kyle Hedges
Date:			Feb 1, 2020
Last Modified:	Feb 1, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

Description: Initializes a 1ms system tick using TIMER 2
*******************************************************************************/ 

//so initializeSysTick: --------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description:	Initializes a 1ms tick that trigger a low prioriy interrupt

//Timer frequency 	= Clock/(Prescaler*(PeriodRegister+1)*Postscaler)
//					= 64MHz/ (32*125*16)
//					= kHz

// Created by:		Kyle Hedges 
// Last Modified:	Feb 1, 2020
//------------------------------------------------------------------------------
#include <xc.h>
#include "hardwareDefinitions.h"

void initializeSysTick(void)
{
	//Timer2 Control Register
	T2CONbits.ON = 0; // [1] Timer is off
	T2CONbits.CKPS = 0b101;// [6:4] prescaler of 32 
	T2CONbits.OUTPS = 0b1111; //[3:0] postscaler of 16
	
	//Timer Counter Register
	//T2TMR // [7:0] 
	
	
	//Timer Period Register
	T2PR = 124; //[7:0] Timer rolls over when this valued is reached
	
	//Timer Hardware Limit Control Register
	T2HLTbits.PSYNC = 0; // [7] Prescaler not synced (needed if reading counter)
	T2HLTbits.CKPOL = 0; // [6] Timer increments on the rising edge on the clock
	//Synchronzing the ON bit prevents potential glitches
	T2HLTbits.CKSYNC = 1; // [5] ON bit synchronized to timer clock input
	T2HLTbits.MODE = 0b00000; // [4:0] Free running mode. Software controlled
	
	//Timer Clock Source Selection
	T2CLKCON = 0b00011; // [4:0] HFINTOSC (64MHz) clock source

	//Timer External Reset Signal Selection Register (Unused)
	//T2RST = 0b000000 // [5:0] Pin selected by PPS
	
	
	//Peripheral Interrupt Request Register
	PIR3bits.TMR2IF = 0; // [3] Clear interrupt flag
	
	//Peripheral Interrupt Priority Register 3
	IPR3bits.TMR2IP = 0; // [3] Interrupt is low priority
	
	//Peripheral Interrupt Enable Register 3
	PIE3bits.TMR2IE = 1; // [3] Enable timer interrupt
	
	T2CONbits.ON = 1; //Enable timer 
}


