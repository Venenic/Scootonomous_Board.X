/*******************************************************************************
File:			loadCells.h
Authors:		Kyle Hedges
Date:			Jan 27, 2020
Last Modified:	Mar. 14, 2020
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

//NUM/DENOM = Alpha = 7/8 = .875 for load cell moving average
#define LOAD_CELL_ALPHA_NUM 7
#define LOAD_CELL_ALPHA_DENOM 8 //Must be base two value;


#define LOAD_CELL_SUM_THRESHOLD 200000 //(Very) Roughly 500g
#define X_ZERO_BOUND 2 //Roughly num
#define Y_ZERO_BOUND 1

//Global variables
volatile unsigned char transmitTimer;
volatile unsigned char sampleTimer;
volatile unsigned char debounceTimer;

loadCellData_t loadCellData[NUMBER_OF_LOAD_CELLS];
loadCellData_t loadCellSum = 0;
loadCellData_t loadCellRawX = 99;
loadCellData_t loadCellRawY = 99;
loadCellData_t loadCellAvgX = 99;
loadCellData_t loadCellAvgY = 99;
loadCellData_t relativePosX = 99;
loadCellData_t relativePosY = 99;

int24String dataString24 = "SXXXXXXX";
int16String dataString16 = "SXXXXX";
int8String dataString8 = "SXXX";

//Function prototypes
void transmitLoadCellData(void);

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
    
    bool newLoadCellData = true; //Ensures data is only used once
    
	motorSpeed motorSpeeds[NUMBER_OF_MOTORS];
	
	//Initialize motor speeds to 0
	motor motorsOut[4];
    
    //Zeroing variables
    char zeroSuccessCount = 0;
    loadCellData_t loadCellRefX = 99;
    loadCellData_t loadCellRefY = 99;
 
    char mode = STOPPED;
    char buttonReadOld = 0;
    char buttonState = ENABLE_PB_RELEASED;
    
    initializeHardware(); 
    
    sendString("Startup Complete");
    while(1)                                               
    { 
        //Debouncing for the enable button
        if(debounceTimer >= 1){
            debounceTimer = 0;
            //Only updates state if it is consistent across a time period
            if(ENABLE_PB_IN == buttonReadOld) buttonState = ENABLE_PB_IN;
            
            buttonReadOld = ENABLE_PB_IN;
        }
        
        //Added for encoder testing
        if(buttonState == ENABLE_PB_PRESSED){
            updateMotorSpeeds(FORWARD,200,200,200,200);
        }
        else updateMotorSpeeds(STOP,200,200,200,200);
        
        
        if(pollLoadCells(loadCellData))
        { 
            loadCellSum = loadCellData[0] + loadCellData[1] + loadCellData[2] + loadCellData[3];
            
            //Uses a moving average
            loadCellRawX = ((loadCellData[LOAD_CELL_FR]+loadCellData[LOAD_CELL_BR])<<7)/loadCellSum;
            loadCellAvgX = loadCellRawX + (((loadCellAvgX-loadCellRawX)*LOAD_CELL_ALPHA_NUM)>> LOAD_CELL_ALPHA_DENOM);
            
            loadCellRawY = ((loadCellData[LOAD_CELL_FR]+loadCellData[LOAD_CELL_FL])<<7)/loadCellSum;
            loadCellAvgY = loadCellRawY + (((loadCellAvgY-loadCellRawY)*LOAD_CELL_ALPHA_NUM)>> LOAD_CELL_ALPHA_DENOM);
            
            newLoadCellData = true;
        }
		
		if(pollEncoder(motorSpeeds))
        { 
			//Data has been updated           
		}	
        
        if(transmitTimer >= 100) //Send data every 100ms
        { 
            transmitTimer = 0;
            for(char i = 0; i < 4; i++)
             {
                sendString(",");
                convert24Bit(motorSpeeds[i], dataString24, SIGNED);
                sendString(dataString24);	
             }
            sendString("\r\n");
        }
        
        switch(mode){  
           case(STOPPED):
               //stuff
                //transmitLoadCellData();
                if(buttonState == ENABLE_PB_PRESSED && loadCellSum > LOAD_CELL_SUM_THRESHOLD)
                {
                   sendString("Zeroing");
                   //Transition to ZEROING mode  
                   mode = ZEROING;
                   STATUS_LED_R_OUT = STATUS_LED_OFF;
                   STATUS_LED_Y_OUT = STATUS_LED_ON;
                   transmitTimer = 0;
                   //sendString("\r\nCLEARSHEET\r\n");
                   //sendString("LABEL,Sample Time,Cell 1,Cell 2,Cell 3,Cell 4,Spd 1,Spd 2,Spd 3,Spd 4\r\n");
                }
                break;
               
            case(ZEROING):
                //Test for steady state
                if(newLoadCellData == true)
                {
                    if(loadCellRawX >= (loadCellAvgX-X_ZERO_BOUND) 
                            && loadCellRawX <= (loadCellAvgX + X_ZERO_BOUND)
                            && loadCellRawY >= (loadCellAvgY - Y_ZERO_BOUND)
                            && loadCellRawY <= (loadCellAvgY + Y_ZERO_BOUND)) 
                    {
                        sendString("Pass \r\n");
                        zeroSuccessCount++;
                    }
                    else
                    {
                        sendString("Fail \r\n");
                        zeroSuccessCount = 0;
                    }
                    
                    //transmitLoadCellData();
                }
                   
  
                if(buttonState == ENABLE_PB_RELEASED || loadCellSum < LOAD_CELL_SUM_THRESHOLD)
                {
                    //Switch to stopped
                     mode = STOPPED;
                    STATUS_LED_Y_OUT = STATUS_LED_OFF;
                    STATUS_LED_R_OUT = STATUS_LED_ON;
                    
                }
                else if(zeroSuccessCount >= 10)
                {
                    sendString("Reference point found\r\n");
                    sendString("CLEARSHEET\r\n");
                    sendString("LABEL,Sample Time,Cell 1,Cell 2,Cell 3,Cell 4,Raw X,Raw Y,Avg X,Avg Y,Rel X,Rel Y\r\n");
                    //Transition to RUNNING mode
                    mode = RUNNING;
                    STATUS_LED_Y_OUT = STATUS_LED_OFF;
                    STATUS_LED_G_OUT = STATUS_LED_ON;
                    
                    loadCellRefX = loadCellAvgX;
                    loadCellRefY = loadCellAvgY;
                    zeroSuccessCount = 0;
                }
  
                break;
   
           case(RUNNING): 
               
               if(newLoadCellData)
               {
                    relativePosX = loadCellAvgX - loadCellRefX;
                    relativePosY = loadCellAvgY - loadCellRefY;            
                }
               //transmitLoadCellData();
                
               //Transition to STOPPED
               if(buttonState == ENABLE_PB_RELEASED || loadCellSum < LOAD_CELL_SUM_THRESHOLD){
                   STATUS_LED_G_OUT = STATUS_LED_OFF;
                   STATUS_LED_R_OUT = STATUS_LED_ON;
                    mode = STOPPED;
                    updateMotorSpeeds(STOP,0,0,0,0);
                    relativePosX = 99;
                    relativePosY = 99;
               }
               break;
       }  
        
       newLoadCellData = false;
    }     
    return;
}

void transmitLoadCellData(void)
{
  
        sendString("DATA,TIMER"); //Excel command

        //convert16Bit(sampleTimer, dataString16, UNSIGNED);
                        //sendString(dataString16);
                        //sampleTimer = 0;

        for(char i = 0; i < NUMBER_OF_LOAD_CELLS; i++)
        {
            sendString(",");
            convert24Bit(loadCellData[i], dataString24, SIGNED);
            sendString(dataString24);
        }

        sendString(",");
        convert24Bit(loadCellSum, dataString24, SIGNED);
        sendString(dataString24);    

        sendString(",");
        convert8Bit(loadCellRawX, dataString8, SIGNED);
        sendString(dataString8);

        sendString(",");
        convert8Bit(loadCellRawY, dataString8, SIGNED);
        sendString(dataString8);

        sendString(",");
        convert8Bit(loadCellAvgX, dataString8, SIGNED);
        sendString(dataString8);

        sendString(",");
        convert8Bit(loadCellAvgY, dataString8, SIGNED);
        sendString(dataString8);
        
        sendString(",");
        convert8Bit(relativePosX, dataString8, SIGNED);
        sendString(dataString8);

        sendString(",");
        convert8Bit(relativePosY, dataString8, SIGNED);
        sendString(dataString8);
  
        sendString("\r\n"); 
    //}
                    
                   
                    
}

                   