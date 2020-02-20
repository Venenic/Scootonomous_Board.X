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
					-Hall effect OUTA: A0 (Pin 2)
					-Hall effect OUTB: A1 (Pin 3)
				
					-Motor 2 PWM: RD1 (Pin 20)
					-Uses PWM2, P2
					-Hall effect OUTA: A2 (Pin 4)
					-Hall effect OUTB: A3 (Pin 5)
					
					-Motor 3 PWM: RD2 (Pin 21)
					-Uses PWM3, P1
					-Hall effect OUTA: A4 (Pin 6)
					-Hall effect OUTB: A5 (Pin 7)
					
					-Motor 4 PWM: RD3 (Pin 22)
					-Uses PWM3, P2
					-Hall effect OUTA: A6 (Pin 14)
					-Hall effect OUTB: A7 (Pin 13) (Yes, these are right)
					
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

#define	M1_SPEED_TRIS TRISAbits.TRISA0
#define M1_SPEED_ANSEL ANSELAbits.ANSELA0
#define M1_POS_IOC IOCAPbits.IOCAP0

#define M1_DIR_TRIS TRISAbits.TRISA1 
#define M1_DIR_ANSEL ANSELAbits.ANSELA1
#define M1_DIR_INPUT PORTAbits.RA1




#define M2_TRIS TRISDbits.TRISD1
#define M2_PPS RD1PPS 
#define PWM2_P2_OUT 0x1B //PWM2_P2 output source identifier

#define M3_TRIS TRISDbits.TRISD2
#define M3_PPS RD2PPS 
#define PWM3_P1_OUT 0x1C //PWM3_P1 output source identifier

#define M4_TRIS TRISDbits.TRISD3
#define M4_PPS RD3PPS 
#define PWM3_P2_OUT 0x1D //PWM3_P2 output source identifier

#define HALLEFFECT_TRIS TRISA
#define HALLEFFECT_LAT LATA

void initializePWM2(void);
void initializePWM3(void);
void initializeTimers(void);
void enableMotors(void);
void initializeMotors(void);


#define TIMER_HIGH_BYTE 1
#define TIMER_LOW_BYTE 0

typedef union timerCount {
	uint16_t value;
	char byte[2];
} timerCount;

//Maybe should throw these in a structure
volatile timerCount pulsePeriod[4];
volatile bool datasReady[4];
volatile char direction[4];
volatile bool overflow[4];

void __interrupt(low_priority, irq(IOC), base(8)) hallEffectPulse()
{
	
	if(IOCAFbits.IOCAF0)
	{
		LATEbits.LATE0 = 1;
		//Motor 1 interrupt
		
		T0CON0bits.EN = 0;
		
		//Timer values need to be read/written low byte first
		pulsePeriod[0].byte[TIMER_LOW_BYTE] = TMR0L;
		pulsePeriod[0].byte[TIMER_HIGH_BYTE] = TMR0H;
		if(overflow[0]){
			pulsePeriod[0].value = 0xFFFF;
			overflow[0] = false;
		}
		
		TMR0L = 0;
		TMR0H = 0;
		T0CON0bits.EN = 1;
		
		direction[0] = M1_DIR_INPUT;
		
		datasReady[0] = 1;
		IOCAFbits.IOCAF0 = 0; //Clear interrupt flag
		LATEbits.LATE0 = 0;
	}
	
	if(IOCAFbits.IOCAF1)
	{
		
	}
	
		
}

void __interrupt(low_priority, irq(TMR0), base(8)) pulseOverflow_1()
{
	//Time between pulses has overflowed (Moving very slow)
	overflow[0] = true;
	PIR3bits.TMR0IF = 0; // [7] Clear interrupt flag
}

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
	initializeTimers();
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
    //3200 Clock cycles per PWM period. F = 64MHz/(3071+127+1) = 20kHz 
    PWM2PRHbits.PRH = 12;  // [15:8] High byte 12*256 = 3072
    PWM2PRLbits.PRL = 127; // [7:0] Lower byte 127
	
	//Clock Prescaler Register (Prescaler divides clock signal))
    PWM2CPREbits.CPRE = 0; //[7:0] no prescaler (division of 0+1 = 1)
	
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
	//Period is 1/20kHz = 50us
    //0x0400 = 16. 64MHz/800 = 80kHz = 12.5 us on time fo 25% DC 
    PWM2S1P1Hbits.S1P1H = 0x03; // [15:8] //High byte   768
    PWM2S1P1Lbits.S1P1L = 0x20; // [7:0] //Low byte		32
	PWM2S1P2Hbits.S1P2H = 0x03; // [15:8] //High byte
    PWM2S1P2Lbits.S1P2L = 0x20; // [7:0] //Low byte
	
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

void initializeTimers(void)
{
	//Timer 0 setup:
	//Timer0 Control Register 0 
	T0CON0bits.EN = 0; // [7] Disable Timer 0 for setup;
	T0CON0bits.MD16 = 1; // [4] Configured as a 16 bit timer0
	T0CON0bits.OUTPS = 0b0000; // [3:0] 1:1 Postscaler
	
	//Timer0 Control Register 1
	T0CON1bits.CS = 0b011; // [7:5] HFINTOSC (64MHz) clock source
	T0CON1bits.ASYNC = 1; // [4] Operate in asynchronous mode
	T0CON1bits.CKPS = 0b0010; // [3:0] 1:2 prescaler for 32MHz steps
	
	//Timer0 Period/Count High/Low Registers
	//TMRR0H = 0; // [7:0]
	//TMRR0L = 0; // [7:0]
	
	//Peripheral Interrupt Priority Register 3
	IPR3bits.TMR0IP = LOW_PRIORITY; // [7] Set timer overflow to low priority
	
	//Peripheral Interrupt Enable Register 3
	PIE3bits.TMR0IE = ENABLE_INTERRUPT; // [7] Enable overflow interrupt
	
	T0CON0bits.EN = 1; //Enable Timer0
}

//Yes this is hard coded, but whatever.
void initializeMotors(void)
{
	M1_PWM_TRIS = OUTPUT_PIN;
	M1_PWM_PPS = PWM2_P1_OUT;
	M1_CONTROL1_TRIS = OUTPUT_PIN;
	M1_CONTROL2_TRIS = OUTPUT_PIN;
	
	M1_SPEED_TRIS = INPUT_PIN;
	M1_SPEED_ANSEL = DIGITAL_INPUT_PIN;
	M1_DIR_TRIS = INPUT_PIN;
	M1_DIR_ANSEL = DIGITAL_INPUT_PIN;
	M1_POS_IOC = 1; // Enable rising edge interrupt for pin C0
	
	
	
	//Peripheral Interrupt Request Register 0
	IPR0bits.IOCIP = LOW_PRIORITY; // [7] IOC set to low priority
	
	//Peripheral Interrupt Enable Register 0
	PIE0bits.IOCIE = ENABLE_INTERRUPT; //[7] Enable IOC interrupts
	
	//Initialize
	for(char i = 0; i < 4; i++)
	{
		datasReady[i] = false;
	}
	
	
	//TODO: configure timer0
	//		Test
	//		Copy and past for other four motors
}					

void enableMotors(void)
{
	PWM2CONbits.EN = 1; //[7] Enable the PWM module
	PWM3CONbits.EN = 1; //[7] Enable the PWM module
}

bool pollEncoder(motor *motorData, char encoderNum)
{	
	if(datasReady[encoderNum])
	{
		
		motorData -> pulseTime = pulsePeriod[encoderNum].value;
		
		datasReady[encoderNum] = false;
		
		return true;
		
	}
	
	return false;
}

