/*******************************************************************************
File: 			motorEnocders.cabs
Authors:		Kyle Hedges
Created:		Feb. 20, 2020
Last Modified:	Feb. 20, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

Description: 	This file reads a motor's speed and direction using two Hall
				effect sensors in quadrature. The time between pulses is read
				on the rising edge. The time between rising edges of encoder A
				determines the speed and the state of encoder B at the start of
				a pulse determines direction.
				
Hardware Settings:	
					-Motor 1:
					-Hall effect OUTA: A0 (Pin 2)
					-Hall effect OUTB: A1 (Pin 3)
					
					-Motor 2:
					-Hall effect OUTA: A2 (Pin 4)
					-Hall effect OUTB: A3 (Pin 5)
					
					-Motor 3:
					-Hall effect OUTA: A4 (Pin 6)
					-Hall effect OUTB: A5 (Pin 7)
					
					-Motor 4:
					-Hall effect OUTA: A6 (Pin 14)
					-Hall effect OUTB: A7 (Pin 13) (Yes, these are right)
					
Board Layout (Top View):	1---F---2
							|		|
							3---B---4
*******************************************************************************/

#include <xc.h>
#include <stdbool.h>

#include "hardwareDefinitions.h"
#include "motorEncoders.h"

#define TIMER_HIGH_BYTE 1
#define TIMER_LOW_BYTE 0

typedef struct encoderDataRaw{
	timerCount pulsePeriod;
	bool dataReady;
	char direction;
	bool overflow;
} encoderDataRaw;

volatile encoderDataRaw encoderData1;

volatile timerCount pulsePeriod;

//so encoderPulse_ISR: ---------------------------------------------------------
// Parameters:		low_priority: It is a low priority interrupt
//					irq(IOC): Interrupt vector source is interrupt on change
//					base(8): Base address of the vector table (8 is default)
// Returns:			void 
//
// Description:		Detects the rising edge of a pulse on speed encoders and 
//					measures the time since the last rising edge
//------------------------------------------------------------------------------
void __interrupt(low_priority, irq(IOC), base(8)) encoderPulse_ISR()
{
	if(IOCAFbits.IOCAF0)
	{
		LATEbits.LATE0 = 1;
		//Encoder 1A interrupt
		
		//Timer0 Control Register 0
		T0CON0bits.EN = 0; // [7] Disable timer 0
		//Timer values need to be read/written low byte first
		encoderData1.pulsePeriod.byte[TIMER_LOW_BYTE] = TMR0L;
		encoderData1.pulsePeriod.byte[TIMER_HIGH_BYTE] = TMR0H;
		TMR0L = 0;
		TMR0H = 0;
		T0CON0bits.EN = 1;
		
		if(encoderData1.overflow){
			//If an overflow has been detected
			//Don't create another data point. Reset to overflow incase not read
			encoderData1.pulsePeriod.value = 0xFFFF;
			encoderData1.overflow = false;
		}
		else{
			encoderData1.dataReady = true;
		}
		
		encoderData1.direction = M1_DIR_INPUT;
		
		IOCAFbits.IOCAF0 = 0; //Clear interrupt flag
		LATEbits.LATE0 = 0;
	}
	
	if(IOCAFbits.IOCAF1)
	{
		
	}	
}//eo enocderPulse_ISR----------------------------------------------------------

//so encoderOverflow_ISR: ------------------------------------------------------
// Parameters:		low_priority: It is a low priority interrupt
//					irq(TMR0): Interrupt vector source Timer 0
//					base(8): Base address of the vector table (8 is default)
// Returns:			void 
//
// Description:		Detects that the timer has overflowed
//------------------------------------------------------------------------------
void __interrupt(low_priority, irq(TMR0), base(8)) pulseOverflow_1()
{
	T0CON0bits.EN = 0;
	
	//Time between pulses has overflowed (Moving very slow)
	encoderData1.pulsePeriod.value = 0xFFFF;
	encoderData1.dataReady = true;
	encoderData1.overflow = true;
	
	//Peripheral Interrupt Enable Register 3
	PIR3bits.TMR0IF = 0; // [7] Clear interrupt flag
	
	TMR0L = 0;
	TMR0H = 0;
	T0CON0bits.EN = 1;
}//eo encoderOverflow_ISR-------------------------------------------------------


void initializeEncoders(void)
{
	//Timer 0 setup:
	//Timer0 Control Register 0 
	T0CON0bits.EN = 0; // [7] Disable Timer 0 for setup;
	T0CON0bits.MD16 = 1; // [4] Configured as a 16 bit timer0
	T0CON0bits.OUTPS = 0b0000; // [3:0] 1:1 Postscaler
	
	//Timer0 Control Register 1
	T0CON1bits.CS = 0b011; // [7:5] HFINTOSC (64MHz) clock source
	T0CON1bits.ASYNC = 1; // [4] Operate in asynchronous mode
	T0CON1bits.CKPS = 0b0111; // [3:0] 1:128 prescaler for 500kHz steps
	
	//Timer0 Period/Count High/Low Registers
	//TMRR0H = 0; // [7:0]
	//TMRR0L = 0; // [7:0]
	
	//Peripheral Interrupt Priority Register 3
	IPR3bits.TMR0IP = LOW_PRIORITY; // [7] Set timer overflow to low priority
	
	//Peripheral Interrupt Enable Register 3
	PIE3bits.TMR0IE = ENABLE_INTERRUPT; // [7] Enable overflow interrupt
	
	M1_ENCODER_TRIS = INPUT_PIN;
	M1_ENCODER_ANSEL = DIGITAL_INPUT_PIN;
	M1_DIR_TRIS = INPUT_PIN;
	M1_DIR_ANSEL = DIGITAL_INPUT_PIN;
	M1_POS_IOC = 1; // Enable rising edge interrupt for pin C0
	
	//Peripheral Interrupt Request Register 0
	IPR0bits.IOCIP = LOW_PRIORITY; // [7] IOC set to low priority
	
	//Initialize
	encoderData1.dataReady = false;
	
	//Peripheral Interrupt Enable Register 0
	PIE0bits.IOCIE = ENABLE_INTERRUPT; //[7] Enable IOC interrupts
	
	T0CON0bits.EN = 1; //Enable Timer0
}


//so pollEncoder: ------------------------------------------------------
// Parameters:		*currentEncoder: A pointer to a structure to hold the data
//					
// Returns:			void 
//
// Description:		Copies over the raw data to synchronize it
//------------------------------------------------------------------------------
bool pollEncoder(encoderData *currentEncoder)
{	
	if(encoderData1.dataReady)
	{	
		currentEncoder[0].pulsePeriod.value = encoderData1.pulsePeriod.value;
		currentEncoder[0].direction = encoderData1.direction;
		
		encoderData1.dataReady = false;
		
		return true;
		
	}
	
	return false;
}
