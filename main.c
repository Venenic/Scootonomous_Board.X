/*
 * File:   main.c
 * Author: khedg
 *
 * Created on January 27, 2020, 8:43 PM
 */


#include <xc.h>

#include "device_config.h"
#include "initializeHardware.h"
#include "loadCells.h"
#include "serialOutput.h"
#include "initializeSysTick.h" 
#include "hardwareDefinitions.h"

volatile bool transmitString = false;
volatile int transmitTimer = 0;
//Interrupt function for ADCPWM
//D0 is only being used as a test output for the system tick

//Default interrupt case. Should never be called
void __interrupt(low_priority, irq(TMR2), base(8)) sysTick()
{
    LATDbits.LATD0 = !LATDbits.LATD0;
    
    transmitTimer ++;
    if(transmitTimer == 1000){
        enableADC_CLK();
        transmitString = true;
        transmitTimer = 0;
    }
    
	//Peripheral Interrupt Request Register
	PIR3bits.TMR2IF = 0; // [3] Clear interrupt flag
}

//Default interrupt case. Should never be called
void __interrupt(low_priority, irq(default), base(8)) Default()
{
  
}

//Note: Use unsigned short long to hold load cell ADC values.
void main(void) {   
  
    TRISDbits.TRISD0 = OUTPUT_PIN;
    LATDbits.LATD0 = 1;
    
	char hello_world [] = "Hello World \n";
  
    initializeHardware(); 

    while(1)                                               
    {
        if(transmitString)
        {
            bool stuff = sendString(hello_world);
        
            if(stuff == true) transmitString = false;
        }
    }
    return;
}
