/*
 * File:   main.c
 * Author: khedg
 *
 * Created on January 27, 2020, 8:43 PM
 */


#include <xc.h>

#include "device_config.h"
#include "initializeHardware.h"

//Interrupt function for ADCPWM
void __interrupt(low_priority,irq(IOC),base(8)) Default_ISR_3()
{
    IOCBFbits.IOCBF0 = 0;
}

//Note: Use unsigned short long to hold load cell ADC values.
void main(void) {
    
  
    
    char hello_world [] = "Hello World \n";
    
    initializeHardware();
    int i = 0;
    
    while (hello_world[i] != '\0')
    {
         while(PIR4bits.U1TXIF == 0) //Wait for the transmit buffer to empty
        {
        }
        
        U1TXB = hello_world[i];
        i++;
    }
    
    char j = 1;
    while(1)                                               
    {
        int i = 0;
    
        while (hello_world[i] != '\0')
        {
             while(PIR4bits.U1TXIF == 0) //Wait for the transmit buffer to empty
            {
            }

            U1TXB = hello_world[i];
            i++;
        }
    }
    return;
}
