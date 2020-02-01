/*
 * File:   main.c
 * Author: khedg
 *
 * Created on January 27, 2020, 8:43 PM
 */


#include <xc.h>

#include "device_config.h"
#include "initializeHardware.h"

 volatile char point = 0;
 char hello_world [] = "Hello World \n";

//Interrupt function for ADCPWM


void __interrupt(low_priority,irq(U1TX),base(8)) Default_ISR_2()
{
    if(hello_world[point] == '\0')
    {
        point = 0;
    }
    
    U1TXB = hello_world[point];
    point++;
}

//Default interrupt case. Should never be called
void __interrupt(low_priority, irq(default), base(8)) Default()
{
  
}

//Note: Use unsigned short long to hold load cell ADC values.
void main(void) {   
  
    initializeHardware();
      
    char j = 1;
    while(1)                                               
    {
        int i = 0;
        i++;
        
    }
    return;
}
