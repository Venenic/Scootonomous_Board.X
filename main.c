/*******************************************************************************
File:			loadCells.h
Authors:		Kyle Hedges
Date:			Jan 27, 2020
Last Modified:	Feb 19, 2020
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
#include "motorControl.h"

#define ZEROING 0
#define MEASURING 1

volatile bool transmitString = false;
volatile uint16_t transmitTimer = 0;
volatile unsigned int sampleTimer;

//D0 is only being used as a test output for the system tick

//Default interrupt case. 1ms tick
void __interrupt(low_priority, irq(TMR2), base(8)) sysTick()
{
    transmitTimer ++;
	sampleTimer++;
    if(transmitTimer == 1000){
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
     
    sendString("Dummy string\r\n"); //Excel doesn't parse the first message
    sendString("CLEARSHEET\r\n");
    sendString("LABEL,Sample Time,Cell 1,Cell 2,Cell 3,Cell 4\r\n");
   
    sendString("Hello World \r\n");

	//Load cell data variables
    sampleTimer = 0; 
    loadCell loadCellData[NUMBER_OF_LOAD_CELLS];
	motor motorData[4];
    
    TRISEbits.TRISE0 = OUTPUT_PIN;
    LATEbits.LATE0 = 0;

    while(1)                                               
    { 
        if(pollLoadCells(loadCellData))
        {
            LATEbits.LATE0 = 1;
			int24String dataString24 = "SXXXXXXX";
            int16String dataString16 = "SXXXXX";
			
			sendString("DATA,"); //Excel command
			
			convert16Bit(sampleTimer, dataString16);
			sendString(dataString16);
			sampleTimer = 0;
			
			sendString(",");
			
			for(char i = 0; i < NUMBER_OF_LOAD_CELLS; i++)
			{
				convert24Bit(loadCellData[i].rawData, dataString24);
				sendString(dataString24);
				sendString(",");
			}
			
			for(char i = 0; i < 1; i++)
			{
				convert16Bit(motorData[i].pulseTime, dataString16);
				sendString(dataString16);
				sendString(",");
			}
			
			sendString("\r\n");
            
            LATEbits.LATE0 = 0;
        }
		
		for(char i = 0; i < 1; i++)
		{

			if(pollEncoder(&motorData[i],i))
			{
				int j = 0;
				int k = j + 1;
				//Do math here
			}				
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
