/*******************************************************************************
File:			loadCells.h
Authors:		Kyle Hedges
Date:			Jan 27, 2020
Last Modified:	Feb 29 2020
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
#include "motorEncoders.h"

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
	
	int24String dataString24 = "SXXXXXXX";
    int16String dataString16 = "SXXXXX";
     
    sendString("Dummy string\r\n"); //Excel doesn't parse the first message
    sendString("CLEARSHEET\r\n");
    sendString("LABEL,Sample Time,Cell 1,Cell 2,Cell 3,Cell 4,Speed 1, Speed 2 Direction 1 \r\n");
   
    sendString("Hello World \r\n");

	//Load cell data variables
    sampleTimer = 0; 
    loadCell loadCellData[NUMBER_OF_LOAD_CELLS];
	encoderData encoderData[4];
    
	//Sometimes used for debugging
    TRISEbits.TRISE0 = OUTPUT_PIN;
    LATEbits.LATE0 = 0;
	
	//Initialize motor speeds
	motor motorsOut[2];
	
	motorsOut[0].mode = M_FORWARD;
	motorsOut[0].dutyCycle = 255;
    
	motorsOut[1].mode = M_FORWARD;
	motorsOut[1].dutyCycle = 255;
	
	updateMotorSpeed(motorsOut);
    //uint32_t testNum = 0;

    while(1)                                               
    { 
        if(pollLoadCells(loadCellData))
        {
			sendString("DATA,"); //Excel command
			
			convert16Bit(sampleTimer, dataString16, UNSIGNED);
			sendString(dataString16);
			sampleTimer = 0;
			
			sendString(",");
			
			for(char i = 0; i < NUMBER_OF_LOAD_CELLS; i++)
			{
				convert24Bit(loadCellData[i].rawData, dataString24, SIGNED);
				sendString(dataString24);
				sendString(",");
			}
			
			for(char i = 0; i < 2; i++)
			{
				convert16Bit(encoderData[i].pulsePeriod.value, dataString16, UNSIGNED);
				sendString(dataString16);
				sendString(",");
				
				if(encoderData[i].direction == FORWARD) sendString("F");
				else sendString("R");
				
				sendString(",");
			}
			
			sendString("\r\n"); 
        }
		
		for(char i = 0; i < 1; i++)
		{
			if(pollEncoder(encoderData))
			{
				int k = 0;
				int j = k+1;
			}				
		}

        /* 
        if(transmitString)
        {
            testNum+=5000;
            convert24Bit(testNum, dataString24, UNSIGNED); //Write the number after the :
            
            sendString("DATA,"); //Excel command
            sendString(dataString24);
            sendString("\r\n");
            transmitString = false;
        }
        */
          
    }
    return;
}
