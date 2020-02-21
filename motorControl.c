/*******************************************************************************
File:			motorControl.c
Authors:		Kyle Hedges
Date:			Feb 4, 2020
Last Modified:	Feb 16, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

Description: 

Hardware Settings:
					-Motor 1 PWM: RD0 (Pin 19)
					-Uses PWM2, P1

					-Motor 2 PWM: RD1 (Pin 20)
					-Uses PWM2, P2
					
					-Motor 3 PWM: RD2 (Pin 21)
					-Uses PWM3, P1
				
					-Motor 4 PWM: RD3 (Pin 22)
					-Uses PWM3, P2
					
Board Layout (Top View):	1---F---2
							|		|
							3---B---4
*******************************************************************************/ 

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "hardwareDefinitions.h"
#include "motorControl.h"

//Motor 1 pinout
#define M1_PWM_TRIS TRISDbits.TRISD0
#define M1_PWM_PPS RD0PPS //Pin RD0 PPS
#define PWM2_P1_OUT 0x1A //PWM2_P1 PPS output source identifier
#define M1_CONTROL1_TRIS TRISCbits.TRISC0
#define M1_CONTROL2_TRIS TRISCbits.TRISC1
#define M1_CONTROL1 LATCbits.LATC0
#define M1_CONTROL2 LATCbits.LATC1

//Other motor pinouts
#define M2_TRIS TRISDbits.TRISD1
#define M2_PPS RD1PPS 
#define PWM2_P2_OUT 0x1B //PWM2_P2 output source identifier

#define M3_TRIS TRISDbits.TRISD2
#define M3_PPS RD2PPS 
#define PWM3_P1_OUT 0x1C //PWM3_P1 output source identifier

#define M4_TRIS TRISDbits.TRISD3
#define M4_PPS RD3PPS 
#define PWM3_P2_OUT 0x1D //PWM3_P2 output source identifier

void initializePWM2(void);
void initializePWM3(void);
void initializeTimers(void);
void enableMotors(void);
void initializeMotors(void);

//so initializeMotorControl: --------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description:	Initializes a 20KHz PWM signal with 25%DC

// Last Modified:	Feb 4, 2020
//------------------------------------------------------------------------------
void initializeMotorControl(void)
{
	initializePWM2();
	initializePWM3();
	initializeMotors();
	enableMotors();
}


void initializePWM2()
{
	//PRM Control Register
    PWM2CONbits.EN = 0; //[7] PWM module is disabled
    PWM2CONbits.LD = 1; //[2] Period and duty cycle load is enabled (Redundant)
    //PWM2CONbits.ERSPOL = 0; //[1]External Reset is active-high 
    //PWM2CONbits.ERSNOW = 0; //[0]Stop counter at end of period(External reset)
	
	 //Clock source
    PWM2CLKbits.CLK = 0b00011; // [4:0] HFINTOSC source

    //Period register
    //200 Clock cycles per PWM period. F = 4MHz/(199+1) = 20kHz 
	//More clock cycles per period means higher duty cycle percision
    PWM2PRHbits.PRH = 0;  // [15:8] High byte 0
    PWM2PRLbits.PRL = 199; // [7:0] Lower byte 199
	
	//Clock Prescaler Register (Prescaler divides clock signal))
    PWM2CPREbits.CPRE = 15; //[7:0] no prescaler (division of 15+1 = 16)
	
	//Interrupt Register
    //PWM2GIRbits.S1P2IF = 0; // [1] Clear P2 interrupt flag (Unused)
    PWM2GIRbits.S1P1IF = 0; // [0] Clear P1 interrupt flag
	
	//Interrupt Enable Register
    PWM2GIEbits.S1P2IE = DISABLE_INTERRUPT; // [1] P2 match interrupt disabled 
    PWM2GIEbits.S1P1IE = DISABLE_INTERRUPT; // [0] P1 match enabled
	
	//PWM Slice "a" Configuration Register
    //PWM2S1CFGbits.POL2 = 0; // [7] P2 output true is high
    PWM2S1CFGbits.POL1 = 0; // [6] P1 output true is high
    PWM2S1CFGbits.PPEN = 0; // [3] Push-Pull is disabled
    PWM2S1CFGbits.MODE = 0b000; // [2:0] P1,P2 mode is left aligned
	
	//Slice "a" Parameter 1/2 Register
    //With left aligned: number of clock periods for which P1 output is high
	//Period is 200 clock cycles
    //(99+1)/200 = 50% DC 
    PWM2S1P1Hbits.S1P1H = 0x00; // [15:8] //High byte   
    PWM2S1P1Lbits.S1P1L = 0x63; // [7:0] //Low byte		99
	PWM2S1P2Hbits.S1P2H = 0x00; // [15:8] //High byte
    PWM2S1P2Lbits.S1P2L = 0x64; // [7:0] //Low byte
	
	PWM2CONbits.LD = 0; //[2] Period and duty cycle load is disabled 
	 
	/* Following PWM1 registers are not relevant to current implementation:
	
	//External Reset Source
    PWM2ERSbits.ERS = 0b00000; // [4:0] All external resets disabled
	
	//Auto-load Trigger Source Select Register
    PWM2LDSbits.LDS = 0b00000; // [4:0] Auto-load disabled
	
	 //Period Interrupt Postscale Value
    PWM2PIPOSbits.PIPOS = 0; //[7:0] Set postscale value
	
	//Mirror copies of all PWMxLD bits (To synchronize load event)
    PWMLOADbits.MPWM1LD = 0; //[0] No PWM1 values transfers pending
	
	//Mirror copies of all PWMxEN bits (turn on all at once))
    PWMENbits.MPWM1EN = 0; //[0] PWM1 is not enabled
	*/
}

void initializePWM3()
{
	M3_TRIS = OUTPUT_PIN;
	M3_PPS = PWM3_P1_OUT;
	
	M4_TRIS = OUTPUT_PIN;
	M4_PPS = PWM3_P2_OUT;
	
	//PRM Control Register
    PWM3CONbits.EN = 0; //[7] PWM module is disabled
    PWM3CONbits.LD = 1; //[2] Period and duty cycle load is enabled (Redundant)
    //PWM3CONbits.ERSPOL = 0; //[1]External Reset is active-high 
    //PWM3CONbits.ERSNOW = 0; //[0]Stop counter at end of period(External reset)
	
	 //Clock source
    PWM3CLKbits.CLK = 0b00011; // [4:0] HFINTOSC source

    //Period register
    //3200 Clock cycles per PWM period. F = 64MHz/(3071+127+1) = 20kHz 
    PWM3PRHbits.PRH = 12;  // [15:8] High byte 12*256 = 3072
    PWM3PRLbits.PRL = 127; // [7:0] Lower byte 127
	
	//Clock Prescaler Register (Prescaler divides clock signal))
    PWM3CPREbits.CPRE = 0; //[7:0] no prescaler (division of 0+1 = 1)
	
	//Interrupt Register
    //PWM3GIRbits.S1P2IF = 0; // [1] Clear P2 interrupt flag (Unused)
    PWM3GIRbits.S1P1IF = 0; // [0] Clear P1 interrupt flag
	
	//Interrupt Enable Register
    PWM3GIEbits.S1P2IE = DISABLE_INTERRUPT; // [1] P2 match interrupt disabled 
    PWM3GIEbits.S1P1IE = DISABLE_INTERRUPT; // [0] P1 match enabled
	
	
	//PWM Slice "a" Configuration Register
    //PWM3S1CFGbits.POL2 = 0; // [7] P2 output true is high
    PWM3S1CFGbits.POL1 = 0; // [6] P1 output true is high
    PWM3S1CFGbits.PPEN = 0; // [3] Push-Pull is disabled
    PWM3S1CFGbits.MODE = 0b000; // [2:0] P1,P2 mode is left aligned
	
	//Slice "a" Parameter 1/2 Register
    //With left aligned: number of clock periods for which P1 output is high
	//Period is 1/20kHz = 50us
    //0x0400 = 16. 64MHz/800 = 80kHz = 12.5 us on time fo 25% DC 
    PWM3S1P1Hbits.S1P1H = 0x03; // [15:8] //High byte   768
    PWM3S1P1Lbits.S1P1L = 0x20; // [7:0] //Low byte		32
	PWM3S1P2Hbits.S1P2H = 0x03; // [15:8] //High byte
    PWM3S1P2Lbits.S1P2L = 0x20; // [7:0] //Low byte
	
	PWM3CONbits.LD = 0; //[2] Period and duty cycle load is disabled 
	 
	/* Following PWM1 registers are not relevant to current implementation:
	
	//External Reset Source
    PWM3ERSbits.ERS = 0b00000; // [4:0] All external resets disabled
	
	//Auto-load Trigger Source Select Register
    PWM3LDSbits.LDS = 0b00000; // [4:0] Auto-load disabled
	
	 //Period Interrupt Postscale Value
    PWM3PIPOSbits.PIPOS = 0; //[7:0] Set postscale value
	
	//Mirror copies of all PWMxLD bits (To synchronize load event)
    PWMLOADbits.MPWM1LD = 0; //[0] No PWM1 values transfers pending
	
	//Mirror copies of all PWMxEN bits (turn on all at once))
    PWMENbits.MPWM1EN = 0; //[0] PWM1 is not enabled
	*/
}


//Yes this is hard coded, but whatever.
void initializeMotors(void)
{
	M1_PWM_TRIS = OUTPUT_PIN;
	M1_PWM_PPS = PWM2_P1_OUT;
	M1_CONTROL1_TRIS = OUTPUT_PIN;
	M1_CONTROL2_TRIS = OUTPUT_PIN;
}					

void enableMotors(void)
{
	PWM2CONbits.EN = 1; //[7] Enable the PWM module
	PWM3CONbits.EN = 1; //[7] Enable the PWM module
}

void updateMotorSpeed(motor *currentMotor)
{
	PWM2CONbits.LD = 1; //[2] Period and duty cycle load is enabled 	
	PWM2S1P1Lbits.S1P1L = currentMotor[0].dutyCycle;
	
	switch(currentMotor[0].mode){
		
		case M_FORWARD :
			M1_CONTROL1 = 1;
			M1_CONTROL2 = 0;
			break;
			
		case M_REVERSE :
			M1_CONTROL1 = 0;
			M1_CONTROL2 = 1;
			break;
		
		case M_BRAKE :
			M1_CONTROL1 = 1;
			M1_CONTROL2 = 1;
			break;
			
		case M_STOP :
			M1_CONTROL1 = 0;
			M1_CONTROL2 = 0;
			break;
 
	}
	
}



