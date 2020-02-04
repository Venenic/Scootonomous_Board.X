/*******************************************************************************
File:			loadCells.h
Authors:		Kyle Hedges
Date:			Jan 27, 2020
Last Modified:	Feb 4, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

TODO:Insert description here
*******************************************************************************/


#include <xc.h>
#include <stdbool.h>

#include "device_config.h"
#include "initializeHardware.h"
#include "loadCells.h"
#include "serialOutput.h"
#include "initializeSysTick.h" 
#include "hardwareDefinitions.h"
#include "binaryToString.h"

volatile bool transmitString = false;
volatile int transmitTimer = 0;
volatile unsigned int sampleTimer;

char testOutputMessage[] = "Output: SXXXXXXX \n";
char cell1OutputMessage[] = "Output 1: SXXXXXXX \n";
char cell2OutputMessage[] = "Output 1: SXXXXXXX \n";
char cell3OutputMessage[] = "Output 1: SXXXXXXX \n";
char cell4OutputMessage[] = "Output 1: SXXXXXXX \n";

loadCellData cellData;

//D0 is only being used as a test output for the system tick

//Default interrupt case. 1ms tick
void __interrupt(low_priority, irq(TMR2), base(8)) sysTick()
{
    
    
    transmitTimer ++;
	sampleTimer++;
    if(transmitTimer == 1000){
        //LATDbits.LATD0 = !LATDbits.LATD0;
        ///enableADC_CLK(); //For testing purposes only FOR TESTING ONLY
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
    LATDbits.LATD0 = 0;
	
	sampleTimer++;
    
	char hello_world [] = "Hello World \n";
 
    sendString(hello_world);
   

   unsigned char i = 0;
   __int24 testNum = 0;
   while(testOutputMessage[i] != ':') i++;
   convert24Bit(testNum, &testOutputMessage[i+2]); //Write the number after the :
    
    while(1)                                               
    {
        //sendString(hello_world);
        
        if(pollLoadCells(&cellData))
        {
            cellData.sampleTime = sampleTimer;
            sampleTimer = 0;
            
            sendString("Time(ms) = ");
            char temp[] = "12345678 ";
            convert24Bit(cellData.sampleTime, temp);
            sendString(temp);
                    
			//LATDbits.LATD0 = 0;
			
            i = 0;
            while(cell1OutputMessage[i] != ':') i++;
            convert24Bit(cellData.cellData1, &cell1OutputMessage[i+2]);
            if(cell1OutputMessage[i+2] == '-')
            {
                sendString("Stop");
                
            }
            sendString(cell1OutputMessage);
            //LATDbits.LATD0 = 1;
			//sampleTimer++;
        }
         /*  
        if(transmitString)
        {
            testNum++;
            while(testOutputMessage[i] != ':') i++;
            convert24Bit(testNum, &testOutputMessage[i+2]); //Write the number after the :
            
            sendString(testOutputMessage);
            transmitString = false;
        }
           */
    }
    return;
}
