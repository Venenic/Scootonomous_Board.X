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

#define ZEROING 0
#define MEASURING 1

volatile bool transmitString = false;
volatile uint16_t transmitTimer = 0;
volatile uint16_t sampleTimer;

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
    
    loadCellData cellData[10];
    uint8_t dataIndex = 0;
    
    int32_t dataAverage1 = 0;
    int32_t dataAverage2 = 0;
    int32_t dataAverage3 = 0;
    int32_t dataAverage4 = 0;
    uint32_t elapsedTime = 0;
    
    int32_t zeroOffset1 = 0;
    int32_t zeroOffset2 = 0;
    int32_t zeroOffset3 = 0;
    int32_t zeroOffset4 = 0;
    
    uint8_t mode = ZEROING;
    bool zeroed = true;
    sendString("Zeroing\n");
    while(1)                                               
    {
        //sendString(hello_world);
        TRISDbits.TRISD0 = OUTPUT_PIN;
         LATDbits.LATD0 = 0;
        if(pollLoadCells(&cellData[dataIndex]))
        {
            if(mode == ZEROING)
            {
                cellData[dataIndex].sampleTime = sampleTimer;
                sampleTimer = 0;
                
                dataAverage1 += cellData[dataIndex].cellData1;
                dataAverage2 += cellData[dataIndex].cellData2;
                dataAverage3 += cellData[dataIndex].cellData3;
                dataAverage4 += cellData[dataIndex].cellData4;
                
                //Ensure that all four measurements are steady
                if(cellData[dataIndex].cellData1 > zeroOffset1 +125 || cellData[dataIndex].cellData1 < zeroOffset1 -125) zeroed = false;
                if(cellData[dataIndex].cellData2 > zeroOffset2 +125 || cellData[dataIndex].cellData2 < zeroOffset2 -125) zeroed = false;
                if(cellData[dataIndex].cellData3 > zeroOffset3 +125 || cellData[dataIndex].cellData3 < zeroOffset3 -125) zeroed = false;
                if(cellData[dataIndex].cellData4 > zeroOffset4 +125 || cellData[dataIndex].cellData4 < zeroOffset4 -125) zeroed = false;
                
                dataIndex++;
                if(dataIndex == 10)
                {
                    dataIndex = 0;
                
                    zeroOffset1 = dataAverage1  / 10;
                    zeroOffset2 = dataAverage2  / 10;
                    zeroOffset3 = dataAverage3  / 10;
                    zeroOffset4 = dataAverage4  / 10;
                    
                    if(zeroed == true){
                        mode = MEASURING;
                        sendString("Sensors have been zeroed");
                    } else sendString("Zeroing\n");
                    zeroed = true;
                    
                     dataAverage1  = 0;
                    dataAverage2  = 0;
                    dataAverage3  = 0;
                    dataAverage4  = 0;
                    elapsedTime = 0;
                }
                
               
            }else{
                cellData[dataIndex].sampleTime = sampleTimer;
                sampleTimer = 0;

                dataAverage1 += cellData[dataIndex].cellData1 - zeroOffset1;
                dataAverage2 += cellData[dataIndex].cellData2 - zeroOffset2;
                dataAverage3 += cellData[dataIndex].cellData3 - zeroOffset3;
                dataAverage4 += cellData[dataIndex].cellData4 - zeroOffset4;
                elapsedTime += cellData[dataIndex].sampleTime;

                dataIndex++;


                if(dataIndex == 10)
                {
                    LATDbits.LATD0 = 1;
                    dataIndex = 0;

                    dataAverage1  /= 10;
                    dataAverage2  /= 10;
                    dataAverage3  /= 10;
                    dataAverage4  /= 10;

                    int24String dataString = "SXXXXXXX"; 

                    sendString("Time since last sample: ") ;    
                    convert24Bit(elapsedTime, dataString);
                    sendString(dataString);
                    sendString("\n");

                    convert24Bit(dataAverage1, dataString);
                    sendString("1: ");
                    sendString(dataString);
                    sendString("\n");

                    convert24Bit(dataAverage2, dataString);
                    sendString("2: ");
                    sendString(dataString);
                    sendString("\n");

                    convert24Bit(dataAverage3, dataString);
                    sendString("3: ");
                    sendString(dataString);
                    sendString("\n");

                    convert24Bit(dataAverage4, dataString);
                    sendString("4: ");
                    sendString(dataString);
                    sendString("\n");

                    sendString("\n");

                    dataAverage1  = 0;
                    dataAverage2  = 0;
                    dataAverage3  = 0;
                    dataAverage4  = 0;
                    elapsedTime = 0;

                    LATDbits.LATD0 = 0;
                }
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
