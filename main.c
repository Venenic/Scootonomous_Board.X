/*******************************************************************************
File:			loadCells.h
Authors:		Kyle Hedges
Date:			Jan 27, 2020
Last Modified:	Mar. 8, 2020
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
#define ZEROING 1
#define RUNNING 2


volatile unsigned char transmitTimer;
volatile unsigned char sampleTimer;
volatile unsigned char debounceTimer;

//D0 is only being used as a test output for the system tick

//Default interrupt case. 1ms tick
void __interrupt(low_priority, irq(TMR2), base(8)) sysTick()
{
    transmitTimer ++;
	sampleTimer++;
    debounceTimer++;
   
	//Peripheral Interrupt Request Register
	PIR3bits.TMR2IF = 0; // [3] Clear interrupt flag
}

//Default interrupt case. Should never be called
void __interrupt(low_priority, irq(default), base(8)) Default()
{
  
}

//Note: Use unsigned short long to hold load cell ADC values.
void main(void) {  
    
    transmitTimer = 0;
    sampleTimer = 0;
    debounceTimer = 0;
    
    loadCell loadCellData[NUMBER_OF_LOAD_CELLS];
	encoderPulse encoderPulses[NUMBER_OF_MOTORS];
	
	//Initialize motor speeds to 0
	motor motorsOut[4];
 
    char mode = STOPPED;
    char buttonReadOld = 0;
    char buttonState = ENABLE_PB_RELEASED;
    int24String dataString24 = "SXXXXXXX";
    //int16String dataString16 = "SXXXXX";
    
    initializeHardware(); 
    
    sendString("Startup Complete");
    while(1)                                               
    { 
        DEBUG_PIN_OUT = buttonState;
        
        //Debouncing for the enable button
        if(debounceTimer >= 1){
            debounceTimer = 0;
            //Only updates state if it is consistent across a time period
            if(ENABLE_PB_IN == buttonReadOld) buttonState = ENABLE_PB_IN;
            
            buttonReadOld = ENABLE_PB_IN;
        }
        
        if(pollLoadCells(loadCellData)){
			//Data has been updated	
        }
		
		if(pollEncoder(encoderPulses)){
			//Data has been updated
		}	
        
        switch(mode){   
           case(STOPPED):
               //stuff
                
                if(buttonState == ENABLE_PB_PRESSED)
                {
                   //Transition to ZEROING mode  
                   mode = ZEROING;
                   STATUS_LED_R_OUT = STATUS_LED_OFF;
                   STATUS_LED_Y_OUT = STATUS_LED_ON;
                   transmitTimer = 0;
                   sendString("\r\nCLEARSHEET\r\n");
                   sendString("LABEL,Sample Time,Cell 1,Cell 2,Cell 3,Cell 4,Speed 1,Dir 1,Speed 2,Dir 2\r\n");
                }
                break;
               
            case(ZEROING):
                //Other stuff
                if(1){
                    //Transition to RUNNING mode
                    mode = RUNNING;
                    STATUS_LED_Y_OUT = STATUS_LED_OFF;
                    STATUS_LED_G_OUT = STATUS_LED_ON;
                    updateMotorSpeeds(FORWARD,100,100,100,100);
                }
                else 
                break;
   
           case(RUNNING):
              
               if(transmitTimer >= 100) //Send data every 10ms
               {               
                    transmitTimer = 0;
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
                }
               
               //Transition to STOPPED
               if(buttonState == ENABLE_PB_RELEASED){
                   STATUS_LED_G_OUT = STATUS_LED_OFF;
                   STATUS_LED_R_OUT = STATUS_LED_ON;
                    mode = STOPPED;
                    updateMotorSpeeds(STOP,0,0,0,0);
               }
               break;
       }           
    }     
    return;
}
