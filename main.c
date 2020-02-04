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
#include <stdint.h>

#include "device_config.h"
#include "initializeHardware.h"
#include "loadCells.h"
#include "serialOutput.h"
#include "initializeSysTick.h" 
#include "hardwareDefinitions.h"
#include "binaryToString.h"

volatile bool transmitString = false;
volatile uint16_t transmitTimer = 0;
volatile uint16_t sampleTimer;


loadCellData cellData;

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
    
    TRISDbits.TRISD0 = OUTPUT_PIN;
    LATDbits.LATD0 = 0;
	
	sampleTimer =0;
    sendString("Hello World \n\n");
   
    while(1)                                               
    {
        //sendString(hello_world);
        
        if(pollLoadCells(&cellData))
        {
            cellData.sampleTime = sampleTimer;
            sampleTimer = 0;
            
            sendString("Time(ms) = ");
            char temp[] = "12345678 \n";
            convert24Bit(cellData.sampleTime, temp);
            sendString(temp);
                    
			LATDbits.LATD0 = 0;
			
           // signed long average = ((cellData.cellData1)*100)/(cellData.cellData1+cellData.cellData2);
            int24String dataString = "SXXXXXXX";
			convert24Bit(cellData.cellData1, dataString);
			sendString("1: ");
			sendString(dataString);
			sendString("\n");
            
            convert24Bit(cellData.cellData2, dataString);
			sendString("2: ");
			sendString(dataString);
			sendString("\n");
            
            convert24Bit(cellData.cellData3, dataString);
			sendString("3: ");
			sendString(dataString);
			sendString("\n");
            
            convert24Bit(cellData.cellData4, dataString);
			sendString("4: ");
			sendString(dataString);
			sendString("\n");
            
            sendString("\n");
           
            LATDbits.LATD0 = 1;
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
