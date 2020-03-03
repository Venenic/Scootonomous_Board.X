/*******************************************************************************
File: 			motorEnocders.cabs
Authors:		Kyle Hedges
Created:		Feb. 20, 2020
Last Modified:	Mar. 1, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

Description: 	This file reads a motor's speed and direction using two Hall
				effect sensors in quadrature. The time between rising edges of 
				encoder A determines the speed and the state of encoder B at 
				this rising edge determines the direction.
				
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


#define	M1_ENCODER_TRIS TRISAbits.TRISA7
#define M1_ENCODER_ANSEL ANSELAbits.ANSELA7
#define M1_POS_IOC IOCAPbits.IOCAP7
#define M1_IOC_FLAG IOCAFbits.IOCAF7

#define M1_DIR_TRIS TRISAbits.TRISA6 
#define M1_DIR_ANSEL ANSELAbits.ANSELA6
#define M1_DIR_INPUT PORTAbits.RA6


#define	M2_ENCODER_TRIS TRISAbits.TRISA4
#define M2_ENCODER_ANSEL ANSELAbits.ANSELA4
#define M2_POS_IOC IOCAPbits.IOCAP4
#define M2_IOC_FLAG IOCAFbits.IOCAF4

#define M2_DIR_TRIS TRISAbits.TRISA5
#define M2_DIR_ANSEL ANSELAbits.ANSELA5
#define M2_DIR_INPUT PORTAbits.RA5

typedef struct encoderDataRaw{
	timerCount newTimer;
	timerCount oldTimer;
	unsigned char overflowTotal;
	unsigned char overflowCount;
	bool dataReady;
	char direction;
} encoderDataRaw;

volatile encoderDataRaw encoderData1;
volatile encoderDataRaw encoderData2;

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
	
	if(M1_IOC_FLAG)
	{
		//Encoder 1A interrupt
		//Timer values need to be read low byte first. Written high first
		encoderData1.oldTimer.value = encoderData1.newTimer.value;
		encoderData1.newTimer.byte[TIMER_LOW_BYTE] = TMR0L;
		encoderData1.newTimer.byte[TIMER_HIGH_BYTE] = TMR0H;
		encoderData1.overflowTotal = encoderData1.overflowCount;
		encoderData1.overflowCount = 0;
		
		encoderData1.direction = M1_DIR_INPUT;
		
		encoderData1.dataReady = true;

		M1_IOC_FLAG = 0; //Clear interrupt flag
	}
	
	if(M2_IOC_FLAG)
	{
		//Encoder 1A interrupt
		//Timer values need to be read low byte first. Written high first
		encoderData2.oldTimer.value = encoderData2.newTimer.value;
		encoderData2.newTimer.byte[TIMER_LOW_BYTE] = TMR0L;
		encoderData2.newTimer.byte[TIMER_HIGH_BYTE] = TMR0H;
		encoderData2.overflowTotal = encoderData2.overflowCount;
		encoderData2.overflowCount = 0;
		
		encoderData2.direction = M2_DIR_INPUT;
		
		encoderData2.dataReady = true;
		
		M2_IOC_FLAG = 0; //Clear interrupt flag
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
void __interrupt(low_priority, irq(TMR0), base(8)) pulseOverflow_0()
{
	encoderData1.overflowCount++;
	encoderData2.overflowCount++;
	
	//Peripheral Interrupt Enable Register 3
	PIR3bits.TMR0IF = 0; // [7] Clear interrupt flag
}//eo encoderOverflow_ISR-------------------------------------------------------

//so initializeEncoders: ---------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description:		Initializes 4 16-bit timers with step period of 2us
//					Note that Timer 0 configuration is different than the other
//					timers since it is a unique peripheral.
//------------------------------------------------------------------------------
void initializeEncoders(void)
{
	
	//Timer 0 setup start-------------------------------------------------------
	//Timer0 Control Register 0 
	T0CON0bits.EN = 0; // [7] Disable Timer 0 for setup;
	T0CON0bits.MD16 = 1; // [4] Configured as a 16 bit timer0
	T0CON0bits.OUTPS = 0b0000; // [3:0] 1:1 Postscaler
	
	//Timer0 Control Register 1
	T0CON1bits.CS = 0b011; // [7:5] HFINTOSC (64MHz) clock source
	T0CON1bits.ASYNC = 1; // [4] Operate in asynchronous mode
	T0CON1bits.CKPS = 0b0110; // [3:0] 1:64 prescaler for 1MHz steps
	
	//Timer0 Period/Count High/Low Registers
	TMR0L = 0; // [7:0]
	TMR0H = 0; // [7:0]
	
	//Peripheral Interrupt Priority Register 3
	IPR3bits.TMR0IP = LOW_PRIORITY; // [7] Set timer overflow to low priority
	
	//Peripheral Interrupt Enable Register 3
	PIE3bits.TMR0IE = ENABLE_INTERRUPT; // [7] Enable overflow interrupt
	
	//End of Timer 0 setup------------------------------------------------------
	
	
	//Configure input pins and IOC
	M1_ENCODER_TRIS = INPUT_PIN;
	M1_ENCODER_ANSEL = DIGITAL_INPUT_PIN;
	M1_DIR_TRIS = INPUT_PIN;
	M1_DIR_ANSEL = DIGITAL_INPUT_PIN;
	M1_POS_IOC = 1; // Enable rising edge interrupt for pin A0
	
	M2_ENCODER_TRIS = INPUT_PIN;
	M2_ENCODER_ANSEL = DIGITAL_INPUT_PIN;
	M2_DIR_TRIS = INPUT_PIN;
	M2_DIR_ANSEL = DIGITAL_INPUT_PIN;
	M2_POS_IOC = 1; // Enable rising edge interrupt for pin A2
	
	//Peripheral Interrupt Request Register 0
	IPR0bits.IOCIP = LOW_PRIORITY; // [7] IOC set to low priority
	
	//Initialize
	encoderData1.dataReady = false;
	encoderData2.dataReady = false;
	
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
	bool dataUpdated = false;
	
	if(encoderData1.dataReady)
	{	
		currentEncoder[0].pulsePeriod = (encoderData1.newTimer.value + encoderData1.overflowTotal*0x10000) - encoderData1.oldTimer.value;
		currentEncoder[0].direction = encoderData1.direction;
		
		encoderData1.dataReady = false;
		
		dataUpdated = true;
		
	}
	
	if(encoderData2.dataReady)
	{	
		currentEncoder[1].pulsePeriod = (encoderData2.newTimer.value + encoderData2.overflowTotal*0x10000) - encoderData2.oldTimer.value; 
		currentEncoder[1].direction = encoderData2.direction;
		
		encoderData2.dataReady = false;
		
		dataUpdated = true;
		
	}
	
	return dataUpdated;
}
