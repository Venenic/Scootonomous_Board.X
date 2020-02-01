/*
 * File:   main.c
 * Author: khedg
 *
 * Created on January 27, 2020, 8:43 PM
 */


#include <xc.h>

#include "device_config.h"
#include "initializeHardware.h"
#include "loadCells.h"
#include "serialOutput.h"
 


//Interrupt function for ADCPWM

//Default interrupt case. Should never be called
void __interrupt(low_priority, irq(default), base(8)) Default()
{
  
}

//Note: Use unsigned short long to hold load cell ADC values.
void main(void) {   
  
	char hello_world [] = "Hello World \n";
  
    initializeHardware(); 

    while(1)                                               
    {
        bool stuff = sendString(hello_world);
    }
    return;
}
