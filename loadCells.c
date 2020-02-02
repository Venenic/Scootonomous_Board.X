/*******************************************************************************
File:			loadCells.h
Authors:		Kyle Hedges
Date:			Jan 31, 2020
Last Modified:	Feb 2, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

This file handles the operation of the four load cells used to measure the 
center of gravity on the board's surface.

ADC clock:
					-RB0 (Pin 33)
					-PWM1 module
					-500kHz, 0.25us ton
					-Low priority intterupt at end of ton

					Data pins:
					-Load cells [3:0] use RD[4:7] (Pins 27-30) respectively

*******************************************************************************/

#include <xc.h>
#include "loadCells.h"
#include "hardwareDefinitions.h"

volatile unsigned char pulseCount = 0;

//so loadCell_ISR: ------------------------------------------------------
// Parameters:		low_priority: It is a low priority interrupt
//					irq(PWM1): Interrupt vector source is PWM1 
//					base(8): Base address of the vector table (8 is default)
// Returns:			void 
//
// Description:		Reads in the data on the negative edge of the PWM pulse and
//					counts out the number of pulses
//
// Created by:		Kyle Hedges 
// Last Modified:	Feb 1, 2020
//------------------------------------------------------------------------------
void __interrupt(high_priority,irq(PWM1),base(8)) loadCell_ISR()
{
	pulseCount++;
	if(pulseCount == NUMBER_OF_PULSES){
		disableADC_CLK();
		pulseCount = 0;
	}
    PWM1GIRbits.S1P1IF = 0; // [0] Clear P1 interrupt flag
}

//so initializeLoadCells: ------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description: 	Initializes the PWM clock used to access the Hx711 and the 
//					negative edge IOC used to read in the data. Additional pins 
//					are configured as inputs to recieve this data.
//
// 500kHz clock can be generated using the period or prescaler registers
// Currently uses the period register only
// Frequency: F = 64MHz/128 = 500kHz (Period register = 127)
// 				Frequency can be lowered if too fast for data collection
//
// Created by:		Kyle Hedges 
// Last Modified:	Jan 31, 2020
//------------------------------------------------------------------------------
void initializeLoadCells(void)
{
	//Output pin configuration
	ADC_CLK_TRIS = OUTPUT_PIN;
	ADC_CLK_ANSEL = DIGITAL_INPUT_PIN;
	ADC_CLK_PPS = PWM1_P1_OUT; //PWM1_P2 output PPS configuration
	
	//PRM Control Register
    PWM1CONbits.EN = 0; //[7] PWM module is disabled
    PWM1CONbits.LD = 1; //[2] Period and duty cycle load is enabled (Redundant)
    //PWM1CONbits.ERSPOL = 0; //[1]External Reset is active-high 
    //PWM1CONbits.ERSNOW = 0; //[0]Stop counter at end of period(External reset)
	
	 //Clock source
    PWM1CLKbits.CLK = 0b00011; // [4:0] HFINTOSC source

    //Period register
    //128 Clock cycles per PWM period. F = 64MHz/127 = 500kHz
    PWM1PRHbits.PRH = 0;  // [15:8] High byte
    PWM1PRLbits.PRL = 127; // [7:0] Lower byte
	
	//Clock Prescaler Register (Prescaler divides clock signal))
    PWM1CPREbits.CPRE = 0; //[7:0] no prescaler (division of 0+1 = 1)
	
	//Interrupt Register
    //PWM1GIRbits.S1P2IF = 0; // [1] Clear P2 interrupt flag (Unused)
    PWM1GIRbits.S1P1IF = 0; // [0] Clear P1 interrupt flag
	
	//Interrupt Enable Register
    PWM1GIEbits.S1P2IE = DISABLE_INTERRUPT; // [1] P2 match interrupt disabled 
    PWM1GIEbits.S1P1IE = ENABLE_INTERRUPT; // [0] P1 match enabled
	
	//PWM Slice "a" Configuration Register
    //PWM1S1CFGbits.POL2 = 0; // [7] P2 output true is high
    PWM1S1CFGbits.POL1 = 0; // [6] P1 output true is high
    PWM1S1CFGbits.PPEN = 0; // [3] Push-Pull is disabled
    PWM1S1CFGbits.MODE = 0b000; // [2:0] P1,P2 mode is left aligned
	
	//Slice "a" Parameter 1 Register
    //With left aligned: number of clock periods for which P1 output is high
    //0x0400 = 16. 64MHz/16 = 4MHz = 0.25us on time 
    PWM1S1P1Hbits.S1P1H = 0x00; // [15:8] //High byte
    PWM1S1P1Lbits.S1P1L = 0x10; // [7:0] //Low byte
	
	PWM1CONbits.LD = 0; //[2] Period and duty cycle load is disabled 
	
	//Peripheral Interrupt Priority Register 4
	IPR4bits.PWM1IP = HIGH_PRIORITY; // [7] Parameter interrupt is low priority
	
	//Peripheral Interrupt Enable Register 4
	PIE4bits.PWM1IE= ENABLE_INTERRUPT; // [7] Enable parameter interrupts
	
	
	  
	/* Following PWM1 registers are not relevant to current implementation:
	
	//External Reset Source
    PWM1ERSbits.ERS = 0b00000; // [4:0] All external resets disabled
	
	//Auto-load Trigger Source Select Register
    PWM1LDSbits.LDS = 0b00000; // [4:0] Auto-load disabled
	
	 //Period Interrupt Postscale Value
    PWM1PIPOSbits.PIPOS = 0; //[7:0] Set postscale value
	
	//Slice "a" Parameter 2 Register (Unused for this design)
    //With left aligned: number of clock periods for which P2 output is high
    //0x0400 = 64. 64MHz/64 = 1MHz = 1us on time (and 1us off time) for 50%DC
    PWM1S1P2Hbits.S1P2H = 0x00; // [15:8] //High byte
    PWM1S1P2Lbits.S1P2L = 0x40; // [7:0] //Low byte
	
	//Mirror copies of all PWMxLD bits (To synchronize load event)
    PWMLOADbits.MPWM1LD = 0; //[0] No PWM1 values transfers pending
	
	//Mirror copies of all PWMxEN bits (turn on all at once))
    PWMENbits.MPWM1EN = 0; //[0] PWM1 is not enabled
	*/
}


//so enableADC_CLK: ------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description: 	Enables the PWM module
//
// Created by:		Kyle Hedges 
// Last Modified:	Jan 31, 2020
//------------------------------------------------------------------------------
void enableADC_CLK(void)
{
	PWM1CONbits.EN = 1; //[7] Enable the PWM module
}

//so disableADC_CLK: ------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description: 	Disables the PWM module
//
// Created by:		Kyle Hedges 
// Last Modified:	Jan 31, 2020
//------------------------------------------------------------------------------
void disableADC_CLK(void)
{
	PWM1CONbits.EN = 0; //[7] Disable the PWM module
}

