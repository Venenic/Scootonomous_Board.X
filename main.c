/*
 * File:   main.c
 * Author: khedg
 *
 * Created on January 27, 2020, 8:43 PM
 */


#include <xc.h>

#include "device_config.h"
#include "initializeHardware.h"

//Note: Use unsigned short long to hold load cell ADC values.
void main(void) {
    
    initializeHardware();
    int i = 5;
    i += 5;
    
    char j = 1;
    while(1)
    {
        
    }
    return;
}
