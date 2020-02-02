/*******************************************************************************
File:			loadCells.h
Authors:		Kyle Hedges
Date:			Jan 27, 2020
Last Modified:	Feb 2, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

TODO:Insert description here
*******************************************************************************/


#include <xc.h>

#include "device_config.h"
#include "initializeHardware.h"
#include "loadCells.h"
#include "serialOutput.h"
#include "initializeSysTick.h" 
#include "hardwareDefinitions.h"
#include "binaryToString.h"

volatile bool transmitString = false;
volatile int transmitTimer = 0;

char outputMessage[] = "Output num: SXXXXXXX E\n";

//D0 is only being used as a test output for the system tick

//Default interrupt case. 1ms tick
void __interrupt(low_priority, irq(TMR2), base(8)) sysTick()
{
    
    
    transmitTimer ++;
    if(transmitTimer == 10){
        LATDbits.LATD0 = !LATDbits.LATD0;
        //enableADC_CLK();
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
    initializeHardware(); 
    
    TRISDbits.TRISD0 = OUTPUT_PIN;
    LATDbits.LATD0 = 1;
    
	char hello_world [] = "Hello World \n";
 
    __int24 testNum = 0;
   
    
    unsigned char i = 0;
    while(outputMessage[i] != ':') i++;
   convert24Bit(testNum, &outputMessage[i+2]); //Write the number after the :
        
    bool countFlag = false;
    
    while(1)                                               
    {
        
        if(countFlag)
        {
           while(outputMessage[i] != ':') i++;
           convert24Bit(testNum, &outputMessage[i+2]); //Write the number after the :
           countFlag = false;
        }
        
        if(transmitString)
        {
            bool sentString = sendString(outputMessage);
            if(sentString == true){
                testNum --;
                countFlag = true;
                transmitString = false;
            }
        }
    }
    return;
}
