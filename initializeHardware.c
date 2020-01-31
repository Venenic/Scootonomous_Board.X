/****************************************************************
File:		initializeHardware.c
Authors:	Kyle Hedges
Date:		Jan 27, 2020
(c) 2020 Lakehead University

DEVICE:	PIC18F47Q43

This file initializes the PIC hardware
****************************************************************/

#include <xc.h>
#include "initializeHardware.h"
#include "device_config.h"

void initializeInterrupts(void);
void initializeADC_CLK(void);
void initializeOscillator(void);
void initializeUART(void);

void initializeHardware(void)
{
    
    TRISBbits.TRISB0 = OUTPUT_PIN;
    ANSELBbits.ANSELB0 = 0; //[0] Set as digital input for IOC
    //TODO: Give #define names to TRIS bits
    
    TRISBbits.TRISB4 = OUTPUT_PIN;
    
    
    RB0PPS = 0x19; //RBO configured as PWM1_P2 output
    RB4PPS = 0x20; //RB4 configured as TX1 output
    initializeOscillator();
    initializeADC_CLK();
    initializeInterrupts();
    initializeUART();
    
}

//System uses internal oscillator at 64MHz
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
    OSCCON3bits.CSWHOLD = 0; // [7] Will switch clocks if NOSC changes
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
    //By default, all oscillators are disabled
    OSCENbits.EXTOEN = 0; // [7] EXTOSC is enabled by peripheral request
    OSCENbits.HFOEN = 0; // [6] HFINTOSC is enabled by peripheral request
    OSCENbits.MFOEN = 0; // [5] MFOEN is enabled by peripheral request
    OSCENbits.LFOEN = 0; // [4] LFOEN is enabled by peripheral request
    OSCENbits.SOSCEN = 0; // [3] SOSCEN is enabled by peripheral request
    OSCENbits.ADOEN = 0; // [2] ADCRC is enabled by peripheral request

}

//Initialize a 500kHz square wave with 50% Duty Cycle;
//Utilizes PWM module 1, slice 1. Slice 2 is unused
void initializeADC_CLK(void)
{
    //External Reset Source
    PWM1ERSbits.ERS = 0b00000; // [4:0] All external resets disabled
    
    //Clock source
    PWM1CLKbits.CLK = 0b00011; // [4:0] HFINTOSC source
    
    //Auto-load Trigger Source Select Register
    PWM1LDSbits.LDS = 0b00000; // [4:0] Auto-load disabled
    
    //500kHz can be obtained using the period or the prescaler register
    
    //Period register
    //128 Clock cycles per PWM period. F = 64MHz/128 = 500kHz
    PWM1PRHbits.PRH = 0;  // [15:8] High byte
    PWM1PRLbits.PRL = 127; // [7:0] Lower byte
    
    //Clock Prescaler Register (Prescaler divides clock signal))
    PWM1CPREbits.CPRE = 0; //[7:0] no prescaler (division of 0+1 = 1)
    
    //Period Interrupt Postscale Value
    PWM1PIPOSbits.PIPOS = 0; //[7:0] no postscale value
    
    //Interrupt Register
    PWM1GIRbits.S1P2IF = 0; // [1] Clear P2 interrupt flag
    PWM1GIRbits.S1P1IF = 0; // [0] Clear P1 interrupt flag
    
    //Interrupt Enable Register
    PWM1GIEbits.S1P2IE = 0; // [1] P2 interrupt disabled
    PWM1GIEbits.S1P1IE = 0; // [0] P1 interrupt disabled
    
    //PRM Control Register
    PWM1CONbits.EN = 0; //[7] PWM module is disabled
    PWM1CONbits.LD = 0; //[2] Period and duty cycle load disabled
    PWM1CONbits.ERSPOL = 0; //[1] External Reset is active-high 
    PWM1CONbits.ERSNOW = 0; //[0] Stop counter at end of period
    
    //PWM Slice "a" Configuration Register
    PWM1S1CFGbits.POL2 = 0; // [7] P2 output true is high
    PWM1S1CFGbits.POL1 = 0; // [6] P1 output true is high
    PWM1S1CFGbits.PPEN = 0; // [3] Push-Pull is disabled
    PWM1S1CFGbits.MODE = 0b000; // [2:0] P1,P2 mode is left aligned
    
    //Slice "a" Parameter 1 Register
    //With left aligned: number of clock periods for which P1 output is high
    //0x0400 = 64. 64MHz/64 = 1MHz = 1us on time (and 1us off time) for 50%DC
    PWM1S1P1Hbits.S1P1H = 0x00; // [15:8] //High byte
    PWM1S1P1Lbits.S1P1L = 0x40; // [7:0] //Low byte
            
     //Slice "a" Parameter 2 Register (Unneeded for this design)
    //With left aligned: number of clock periods for which P2 output is high
    //0x0400 = 64. 64MHz/64 = 1MHz = 1us on time (and 1us off time) for 50%DC
    PWM1S1P2Hbits.S1P2H = 0x00; // [15:8] //High byte
    PWM1S1P2Lbits.S1P2L = 0x40; // [7:0] //Low byte
    
         
    //Mirror copies of all PWMxLD bits (To synchronize load event)
    PWMLOADbits.MPWM1LD = 0; //[0] No PWM1 values transfers pending
    
    //Mirror copies of all PWMxEN bits (turn on all at once))
    PWMENbits.MPWM1EN = 0; //[0] PWM1 is not enabled
  
    PWM1CONbits.EN = 1; //[7] PWM1 module is enabled  
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
    
    //Peripheral Interrupt Enable Register 0
    PIE0bits.IOCIE = 1; // [7] //Enable interrupt on change 
    
    //Interrupt-on-Change Negative Edge Register Example
    IOCBNbits.IOCBN0 = 1; //Interrupt enabled on pin 0 (ADC PWM output)
    
    //Peripheral Interrupt Request Register 0
    IPR0bits.IOCIP = 0; // [7] Set IOC to low priority
    
    
    //Peripheral Interrupt Request Register 0
    //PIR0bits.IOCIF [7] Read only Interrupt-on-Change Flag
    //Contains all IOC status flags and will only be cleared by clearing all flags
    
    //Interrupt-on-Change Flag Register
    //IOCBFbits.IOCBF0 Set if a negative edge is detected. Must be cleared.
    
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
