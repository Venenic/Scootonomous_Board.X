/*******************************************************************************
File:			motorControl.c
Authors:		Kyle Hedges
Date:			Feb 4, 2020
Last Modified:	Mar 7, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

Description: 	Controls all four motors
				Duty cycle is 0-200

Hardware Settings:
					-Motor 1 Pins: 	PWM: RD1 (Pin 20)
									Control 1: C0 (Pin 18)
									Control 2: D0 (Pin 19)
									
							Modules: PWM2, P1
							
					-Motor 2 Pins: 	PWM: RC0 (Pin 15)
									Control 1: C2 (Pin 17)
									Control 2: C1 (Pin 16)
									
							Modules: PWM1, P1

					-Motor 3 Pins:  PWM: RC6 (Pin 25)
 *                                  Control 1: D4 (Pin 27)
 *                                  Control 2: C7 (Pin 26)
 * 
                            Modules: PWM 2, P2
 * 
 *                  -Motor 3 Pins:  PWM: RD7 (Pin 30)
 *                                  Control 1: D5 (Pin 29)
 *                                  Control 2: D6 (Pin 28)
 * 
 *                          Modules: PWM 1, P2
 
PPS PWM identifiers:
	-PWM1_P1 0x18 (Port C only)
	-PWM1_P2 0x19 
	-PWM2_P1 0x1A (Port D only)
	-PWM2_P2 0x1B
 
Board Layout (Top View):	1---F---3
							|		|
							2---B---4
*******************************************************************************/ 

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "hardwareDefinitions.h"
#include "motorControl.h"

//Motor 1 pinout
#define M1_PWM_TRIS TRISDbits.TRISD1
#define M1_PWM_PPS RD1PPS //Pin RD1 PPS
#define M1_PWM_ID 0x1A //PWM2_P1 PPS output source identifier
#define M1_P_REG PWM2S1P1L //Parameter register (For setting duty cycle)
#define M1_CONTROL1_TRIS TRISCbits.TRISC3
#define M1_CONTROL1 LATCbits.LATC3
#define M1_CONTROL2_TRIS TRISDbits.TRISD0
#define M1_CONTROL2 LATDbits.LATD0

//Motor 2 pinout
#define M2_PWM_TRIS TRISCbits.TRISC0
#define M2_PWM_PPS RC0PPS //Pin RD0 PPS
#define M2_PWM_ID 0x18 //PWM1_P1 PPS output source identifier
#define M2_P_REG PWM1S1P1L
#define M2_CONTROL1_TRIS TRISCbits.TRISC2
#define M2_CONTROL1 LATCbits.LATC2
#define M2_CONTROL2_TRIS TRISCbits.TRISC1
#define M2_CONTROL2 LATCbits.LATC1

//Motor 3 pinout
#define M3_PWM_TRIS TRISCbits.TRISC6
#define M3_PWM_PPS RC6PPS //Pin RD0 PPS
#define M3_PWM_ID 0x19 //PWM1_P2 PPS output source identifier
#define M3_P_REG PWM1S1P2L
#define M3_CONTROL1_TRIS TRISDbits.TRISD4
#define M3_CONTROL1 LATDbits.LATD4
#define M3_CONTROL2_TRIS TRISCbits.TRISC7
#define M3_CONTROL2 LATCbits.LATC7

//Motor 4 pinout
#define M4_PWM_TRIS TRISDbits.TRISD7
#define M4_PWM_PPS RD7PPS //Pin RD0 PPS
#define M4_PWM_ID 0x1B //PWM2_P2 PPS output source identifier
#define M4_P_REG PWM2S1P2L
#define M4_CONTROL1_TRIS TRISDbits.TRISD5
#define M4_CONTROL1 LATDbits.LATD5
#define M4_CONTROL2_TRIS TRISDbits.TRISD6
#define M4_CONTROL2 LATDbits.LATD6



void initializePWM2(void);
void initializePWM1(void);
void initializeTimers(void);
void enableMotors(void);
void initializeMotors(void);

char mode[4];

//so initializeMotorControl: --------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description:	Initializes a 20KHz PWM signal with 25%DC

// Last Modified:	Feb 4, 2020
//------------------------------------------------------------------------------
void initializeMotorControl(void)
{
	initializePWM1();
    initializePWM2();
	//initializePWM3(); Motor 3 and 4 are unused right now
	initializeMotors();
    updateMotorSpeeds(STOP,0,0,0,0);
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
	//More clock cycles per period means higher duty cycle precision
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
    PWM2S1P1Lbits.S1P1L = 0x00; // [7:0] //Low byte		99
	PWM2S1P2Hbits.S1P2H = 0x00; // [15:8] //High byte
    PWM2S1P2Lbits.S1P2L = 0x00; // [7:0] //Low byte
	
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

void initializePWM1()
{	
	//PRM Control Register
    PWM1CONbits.EN = 0; //[7] PWM module is disabled
    PWM1CONbits.LD = 1; //[2] Period and duty cycle load is enabled (Redundant)
    //PWM3CONbits.ERSPOL = 0; //[1]External Reset is active-high 
    //PWM3CONbits.ERSNOW = 0; //[0]Stop counter at end of period(External reset)
	
	 //Clock source
    PWM1CLKbits.CLK = 0b00011; // [4:0] HFINTOSC source

    //Period register
    //3200 Clock cycles per PWM period. F = 64MHz/(3071+127+1) = 20kHz 
    PWM1PRHbits.PRH = 0;  // [15:8] High byte 12*256 = 3072
    PWM1PRLbits.PRL = 199; // [7:0] Lower byte 127
	
	//Clock Prescaler Register (Prescaler divides clock signal))
    PWM1CPREbits.CPRE = 15; //[7:0] no prescaler (division of 0+1 = 1)
	
	//Interrupt Register
    //PWM3GIRbits.S1P2IF = 0; // [1] Clear P2 interrupt flag (Unused)
    PWM1GIRbits.S1P1IF = 0; // [0] Clear P1 interrupt flag
    PWM1GIRbits.S1P2IF = 0; // [0] Clear P1 interrupt flag
	
	//Interrupt Enable Register
    PWM1GIEbits.S1P2IE = DISABLE_INTERRUPT; // [1] P2 match interrupt disabled 
    PWM1GIEbits.S1P1IE = DISABLE_INTERRUPT; // [0] P1 match enabled
	
	
	//PWM Slice "a" Configuration Register
    //PWM3S1CFGbits.POL2 = 0; // [7] P2 output true is high
    PWM1S1CFGbits.POL1 = 0; // [6] P1 output true is high
    PWM1S1CFGbits.PPEN = 0; // [3] Push-Pull is disabled
    PWM1S1CFGbits.MODE = 0b000; // [2:0] P1,P2 mode is left aligned
	
	//Slice "a" Parameter 1/2 Register
    //With left aligned: number of clock periods for which P1 output is high
	//Period is 1/20kHz = 50us
    //0x0400 = 16. 64MHz/800 = 80kHz = 12.5 us on time fo 25% DC 
    PWM1S1P1Hbits.S1P1H = 0x00; // [15:8] //High byte   768
    PWM1S1P1Lbits.S1P1L = 0x00; // [7:0] //Low byte		32
	PWM1S1P2Hbits.S1P2H = 0x00; // [15:8] //High byte
    PWM1S1P2Lbits.S1P2L = 0x00; // [7:0] //Low byte
	
	PWM1CONbits.LD = 0; //[2] Period and duty cycle load is disabled 
	 
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
	M1_PWM_PPS = M1_PWM_ID;
	M1_CONTROL1 = 0; //Initialize to stop
	M1_CONTROL2 = 0;
	M1_CONTROL1_TRIS = OUTPUT_PIN;
	M1_CONTROL2_TRIS = OUTPUT_PIN;
	
	M2_PWM_TRIS = OUTPUT_PIN;
	M2_PWM_PPS = M2_PWM_ID;
	M2_CONTROL1 = 0; //Initialize to stop
	M2_CONTROL2 = 0;
	M2_CONTROL1_TRIS = OUTPUT_PIN;
	M2_CONTROL2_TRIS = OUTPUT_PIN;
	
    M3_PWM_TRIS = OUTPUT_PIN;
	M3_PWM_PPS = M3_PWM_ID;
	M3_CONTROL1 = 0; //Initialize to stop
	M3_CONTROL2 = 0;
	M3_CONTROL1_TRIS = OUTPUT_PIN;
	M3_CONTROL2_TRIS = OUTPUT_PIN;
    
    M4_PWM_TRIS = OUTPUT_PIN;
	M4_PWM_PPS = M4_PWM_ID;
	M4_CONTROL1 = 0; //Initialize to stop
	M4_CONTROL2 = 0;
	M4_CONTROL1_TRIS = OUTPUT_PIN;
	M4_CONTROL2_TRIS = OUTPUT_PIN;
}					

void enableMotors(void)
{
	PWM2CONbits.EN = 1; //[7] Enable the PWM module
	PWM1CONbits.EN = 1; //[7] Enable the PWM module
}

void updateMotorSpeeds(char dir, unsigned char duty1, unsigned char duty2, 
                            unsigned char duty3, unsigned char duty4)
{
    M1_P_REG = duty1;
	M2_P_REG = duty2;
    M3_P_REG = duty3;
    M4_P_REG = duty4;
    
    switch(dir){
        
        case(STOP):
            mode[0] = M_STOP;
            mode[1] = M_STOP;
            mode[2] = M_STOP;
            mode[3] = M_STOP;
            break;
            
        case(BRAKE):
            mode[0] = M_BRAKE;
            mode[1] = M_BRAKE;
            mode[2] = M_BRAKE;
            mode[3] = M_BRAKE;
            
        case(FORWARD):
            mode[0] = M_FORWARD;
            mode[1] = M_FORWARD;
            mode[2] = M_REVERSE;
            mode[3] = M_REVERSE;
            break;
            
        case(REVERSE):
            mode[0] = M_REVERSE;
            mode[1] = M_REVERSE;
            mode[2] = M_FORWARD;
            mode[3] = M_FORWARD;
            break;
            
        case(LEFT):
            mode[0] = M_REVERSE;
            mode[1] = M_REVERSE;
            mode[2] = M_FORWARD;
            mode[3] = M_FORWARD;
            break;
            
        case(RIGHT):
            mode[0] = M_FORWARD;
            mode[1] = M_FORWARD;
            mode[2] = M_REVERSE;
            mode[3] = M_REVERSE;
            break;
    }
            
            
        PWM2CONbits.LD = 1; //[2] Period and duty cycle load is enabled 	
        PWM1CONbits.LD = 1; //[2] Period and duty cycle load is enabled 
	
        //Motor 1 mode
    switch(mode[0]){

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

    //Motor 2 mode
    switch(mode[1]){

        case M_FORWARD :
            M2_CONTROL1 = 1;
            M2_CONTROL2 = 0;
            break;

        case M_REVERSE :
            M2_CONTROL1 = 0;
            M2_CONTROL2 = 1;
            break;

        case M_BRAKE :
            M2_CONTROL1 = 1;
            M2_CONTROL2 = 1;
            break;

        case M_STOP :
            M2_CONTROL1 = 0;
            M2_CONTROL2 = 0;
            break;
    }

    //Motor 3 mode
    switch(mode[2]){

        case M_FORWARD :
            M3_CONTROL1 = 1;
            M3_CONTROL2 = 0;
            break;

        case M_REVERSE :
            M3_CONTROL1 = 0;
            M3_CONTROL2 = 1;
            break;

        case M_BRAKE :
            M3_CONTROL1 = 1;
            M3_CONTROL2 = 1;
            break;

        case M_STOP :
            M3_CONTROL1 = 0;
            M3_CONTROL2 = 0;
            break;
    }

    //Motor 4 mode
    switch(mode[3]){

        case M_FORWARD :
            M4_CONTROL1 = 1;
            M4_CONTROL2 = 0;
            break;

        case M_REVERSE :
            M4_CONTROL1 = 0;
            M4_CONTROL2 = 1;
            break;

        case M_BRAKE :
            M4_CONTROL1 = 1;
            M4_CONTROL2 = 1;
            break;

        case M_STOP :
            M4_CONTROL1 = 0;
            M4_CONTROL2 = 0;
            break;
    }          
}

