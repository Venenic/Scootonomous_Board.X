/*******************************************************************************
File:			loadCells.h
Authors:		Kyle Hedges
Date:			Jan 27, 2020
Last Modified:	Mar. 5, 2020
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

//Different modes
#define STOPPED 0
#define ZEROING 2
#define RUNNING 1

volatile bool transmitString = false;
volatile uint16_t transmitTimer = 0;
volatile unsigned int sampleTimer;

//D0 is only being used as a test output for the system tick

//Default interrupt case. 1ms tick
void __interrupt(low_priority, irq(TMR2), base(8)) sysTick()
{
    transmitTimer ++;
	sampleTimer++;
    if(transmitTimer == 100){
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
     
    sendString("\r\nCLEARSHEET\r\n");
    sendString("LABEL,Sample Time,Cell 1,Cell 2,Cell 3,Cell 4,Speed 1,Dir 1,Speed 2,Dir 2\r\n");
   
    sendString("Hello World \r\n");

	//Load cell data variables
    sampleTimer = 0; 
    loadCell loadCellData[NUMBER_OF_LOAD_CELLS];
	encoderPulse encoderPulses[4];
    
	//Sometimes used for debugging
    TRISAbits.TRISA1 = OUTPUT_PIN;
    SLRCONAbits.SLRA1 = MAX_SLEW_RATE;
    LATAbits.LATA1 = 0;
	
	//Initialize motor speeds
	motor motorsOut[4];
	
    for(char i = 0; i < NUMBER_OF_MOTORS; i++)
    {
        motorsOut[i].mode = M_FORWARD;
        motorsOut[i].dutyCycle = 200;
    }

	updateMotorSpeed(motorsOut);
    
    char mode = STOPPED;
    
    TRISAbits.TRISA0 = INPUT_PIN;
    ANSELAbits.ANSELA0 = DIGITAL_INPUT_PIN;
    
    
    //uint32_t testNum = 0;

    while(1)                                               
    { 
        //LATAbits.LATA1 = PORTBbits.RB0;
        //TODO initialize sensor values to 0
        if(pollLoadCells(loadCellData))
        {
			//Data has been updated	
        }
		
		if(pollEncoder(encoderPulses))
		{
			//Data has been updated
		}	

        if(transmitString)
        {
          
			sendString("DATA,TIMER"); //Excel command
		   
			//convert16Bit(sampleTimer, dataString16, UNSIGNED);
			//sendString(dataString16);
			//sampleTimer = 0;

			for(char i = 0; i < NUMBER_OF_LOAD_CELLS; i++)
			{
                sendString(",");
				convert24Bit(loadCellData[i].rawData, dataString24, SIGNED);
				sendString(dataString24);
			}
			
            
			for(char i = 0; i < 4; i++)
			{
                sendString(",");
				convert24Bit(encoderPulses[i].pulsePeriod, dataString24, SIGNED);
				sendString(dataString24);		
			}
			
			sendString("\r\n");
		   
           transmitString = false;
   
        }  
        
        if(PORTAbits.RA0 == 0)
        {
            mode = STOPPED;
        }
        else mode = RUNNING;
        
        
        switch(mode){   
           case(STOPPED):
               //stuff
               motorsOut[0].dutyCycle = 0;
               motorsOut[1].dutyCycle = 0;
               motorsOut[2].dutyCycle = 0;
               motorsOut[3].dutyCycle = 0;
               break;

           case(ZEROING):
               //Other stuff
               mode = RUNNING;
               break;

           case(RUNNING):
               //More stuff
               motorsOut[0].dutyCycle = 150;
               motorsOut[1].dutyCycle = 150;
               motorsOut[2].dutyCycle = 150;
               motorsOut[3].dutyCycle = 150;
               break;
       }
        
       updateMotorSpeed(motorsOut);            
    }     
    return;
}
