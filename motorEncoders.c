/*******************************************************************************
File: 			motorEnocders.cabs
Authors:		Kyle Hedges
Created:		Feb. 20, 2020
Last Modified:	Mar. 8, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

Description: 	This file reads a motor's speed and direction using two Hall
				effect sensors in quadrature. The time between rising edges of 
				encoder A determines the speed and the state of encoder B at 
				this rising edge determines the direction.
				
Hardware Settings:	
					-Motor 1:
					-Hall effect OUTA: A7 (Pin 13)
					-Hall effect OUTB: A6 (Pin 14)  (Yes, these are right)
					
					-Motor 2:
					-Hall effect OUTA: A4 (Pin 6)
					-Hall effect OUTB: A5 (Pin 7)
					
					-Motor 3:
					-Hall effect OUTA: B0 (Pin 33)
					-Hall effect OUTB: B1 (Pin 34)
					
					-Motor 4:
					-Hall effect OUTA: B2 (Pin 35)
					-Hall effect OUTB: B3 (Pin 36)
					
Board Layout (Top View):	1---F---3
							|		|
							2---B---4
*******************************************************************************/

#include <xc.h>
#include <stdbool.h>

#include "hardwareDefinitions.h"
#include "motorEncoders.h"

#define TIMER_HIGH_BYTE 1
#define TIMER_LOW_BYTE 0

//Device pinouts:
//Motor 1
#define	M1_ENCODER_TRIS TRISAbits.TRISA7
#define M1_ENCODER_ANSEL ANSELAbits.ANSELA7
#define M1_POS_IOC IOCAPbits.IOCAP7
#define M1_IOC_FLAG IOCAFbits.IOCAF7
#define M1_IOC_INLVL INLVLAbits.INLVLA7

#define M1_DIR_TRIS TRISAbits.TRISA6 
#define M1_DIR_ANSEL ANSELAbits.ANSELA6
#define M1_DIR_INPUT PORTAbits.RA6

//Motor 2
#define	M2_ENCODER_TRIS TRISAbits.TRISA4
#define M2_ENCODER_ANSEL ANSELAbits.ANSELA4
#define M2_POS_IOC IOCAPbits.IOCAP4
#define M2_IOC_FLAG IOCAFbits.IOCAF4
#define M2_IOC_INLVL INLVLAbits.INLVLA4

#define M2_DIR_TRIS TRISAbits.TRISA5
#define M2_DIR_ANSEL ANSELAbits.ANSELA5
#define M2_DIR_INPUT PORTAbits.RA5

//Motor 3
#define	M3_ENCODER_TRIS TRISBbits.TRISB0
#define M3_ENCODER_ANSEL ANSELBbits.ANSELB0
#define M3_POS_IOC IOCBPbits.IOCBP0
#define M3_IOC_FLAG IOCBFbits.IOCBF0
#define M3_IOC_INLVL INLVLBbits.INLVLB0

#define M3_DIR_TRIS TRISBbits.TRISB1
#define M3_DIR_ANSEL ANSELBbits.ANSELB1
#define M3_DIR_INPUT PORTBbits.RB1

//Motor 4
#define	M4_ENCODER_TRIS TRISBbits.TRISB2
#define M4_ENCODER_ANSEL ANSELBbits.ANSELB2
#define M4_POS_IOC IOCBPbits.IOCBP2
#define M4_IOC_FLAG IOCBFbits.IOCBF2
#define M4_IOC_INLVL INLVLBbits.INLVLB2

#define M4_DIR_TRIS TRISBbits.TRISB3
#define M4_DIR_ANSEL ANSELBbits.ANSELB3
#define M4_DIR_INPUT PORTBbits.RB3


typedef struct encoderDataRaw{
	timerCount newTimer;
	timerCount oldTimer;
	unsigned char overflowTotal;
	unsigned char overflowCount;
	bool dataReady;
	char direction;
} encoderDataRaw;

volatile encoderDataRaw encoderData[4];

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
		encoderData[0].oldTimer.value = encoderData[0].newTimer.value;
		encoderData[0].newTimer.byte[TIMER_LOW_BYTE] = TMR0L;
		encoderData[0].newTimer.byte[TIMER_HIGH_BYTE] = TMR0H;
		encoderData[0].overflowTotal = encoderData[0].overflowCount;
		encoderData[0].overflowCount = 0;
		
		encoderData[0].direction = M1_DIR_INPUT;
		
		encoderData[0].dataReady = true;

		M1_IOC_FLAG = 0; //Clear interrupt flag
        
	}
	
	if(M2_IOC_FLAG)
	{      
		//Encoder 2A interrupt
		//Timer values need to be read low byte first. Written high first
		encoderData[1].oldTimer.value = encoderData[1].newTimer.value;
		encoderData[1].newTimer.byte[TIMER_LOW_BYTE] = TMR0L;
		encoderData[1].newTimer.byte[TIMER_HIGH_BYTE] = TMR0H;
		encoderData[1].overflowTotal = encoderData[1].overflowCount;
		encoderData[1].overflowCount = 0;
		
		encoderData[1].direction = M2_DIR_INPUT;
		
		encoderData[1].dataReady = true;
		
		M2_IOC_FLAG = 0; //Clear interrupt flag
	}	
    
    if(M3_IOC_FLAG)
	{        
		//Encoder 3A interrupt
		//Timer values need to be read low byte first. Written high first
		encoderData[2].oldTimer.value = encoderData[2].newTimer.value;
		encoderData[2].newTimer.byte[TIMER_LOW_BYTE] = TMR0L;
		encoderData[2].newTimer.byte[TIMER_HIGH_BYTE] = TMR0H;
		encoderData[2].overflowTotal = encoderData[2].overflowCount;
		encoderData[2].overflowCount = 0;
		
		encoderData[2].direction = M3_DIR_INPUT;
		
		encoderData[2].dataReady = true;
        
		M3_IOC_FLAG = 0; //Clear interrupt flag      
	}
	
	if(M4_IOC_FLAG)
	{   
		//Encoder 1A interrupt
		//Timer values need to be read low byte first. Written high first
		encoderData[3].oldTimer.value = encoderData[3].newTimer.value;
		encoderData[3].newTimer.byte[TIMER_LOW_BYTE] = TMR0L;
		encoderData[3].newTimer.byte[TIMER_HIGH_BYTE] = TMR0H;
		encoderData[3].overflowTotal = encoderData[3].overflowCount;
		encoderData[3].overflowCount = 0;
		
		encoderData[3].direction = M4_DIR_INPUT;
		
		encoderData[3].dataReady = true;
        
		M4_IOC_FLAG = 0; //Clear interrupt flag        
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
	encoderData[0].overflowCount++;
	encoderData[1].overflowCount++;
    encoderData[2].overflowCount++;
	encoderData[3].overflowCount++;
	
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
	M1_POS_IOC = 1; // Enable rising edge interrupt
    M1_IOC_INLVL = ST_INPUT_PIN;
	
	M2_ENCODER_TRIS = INPUT_PIN;
	M2_ENCODER_ANSEL = DIGITAL_INPUT_PIN;
	M2_DIR_TRIS = INPUT_PIN;
	M2_DIR_ANSEL = DIGITAL_INPUT_PIN;
	M2_POS_IOC = 1; // Enable rising edge interrupt
    M2_IOC_INLVL = ST_INPUT_PIN;
    
    M3_ENCODER_TRIS = INPUT_PIN;
	M3_ENCODER_ANSEL = DIGITAL_INPUT_PIN;
	M3_DIR_TRIS = INPUT_PIN;
	M3_DIR_ANSEL = DIGITAL_INPUT_PIN;
	M3_POS_IOC = 1; // Enable rising edge interrupt
    M3_IOC_INLVL = ST_INPUT_PIN;
    
    M4_ENCODER_TRIS = INPUT_PIN;
	M4_ENCODER_ANSEL = DIGITAL_INPUT_PIN;
	M4_DIR_TRIS = INPUT_PIN;
	M4_DIR_ANSEL = DIGITAL_INPUT_PIN;
	M4_POS_IOC = 1; // Enable rising edge interrupt
    M4_IOC_INLVL = ST_INPUT_PIN;
	
	//Peripheral Interrupt Request Register 0
	IPR0bits.IOCIP = LOW_PRIORITY; // [7] IOC set to low priority
	
	//Initialize
    for(char i = 0; i < 4; i++)
    {
        encoderData[i].newTimer.value = 0;
        encoderData[i].oldTimer.value = 0;
        encoderData[i].overflowCount = 0;
        encoderData[i].overflowTotal = 0;
        encoderData[i].direction = ENCODER_FORWARD;
        encoderData[i].dataReady = false;
	}
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
bool pollEncoder(encoderPulse *currentEncoder)
{	
	bool dataUpdated = false;
	for(char i = 0; i < 4; i++)
    {
        if(encoderData[i].dataReady)
        {	
            currentEncoder[i].pulsePeriod = ((encoderData[i].newTimer.value + encoderData[i].overflowTotal*0x10000) - encoderData[i].oldTimer.value);
            currentEncoder[i].direction = encoderData[i].direction;
		
            encoderData[i].dataReady = false;
		
            dataUpdated = true;		
        }    
    }
	
	return dataUpdated;
}
