/*******************************************************************************
File: 			motorEnocders.cabs
Authors:		Kyle Hedges
Created:		Feb. 20, 2020
Last Modified:	Feb. 29, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

Description: 	This file reads a motor's speed and direction using two Hall
				effect sensors in quadrature. The time between pulses is read
				on the rising edge. The time between rising edges of encoder A
				determines the speed and the state of encoder B at the start of
				a pulse determines direction.
				
Hardware Settings:	
					-Motor 1:
					-Hall effect OUTA: A0 (Pin 2)
					-Hall effect OUTB: A1 (Pin 3)
					
					-Motor 2:
					-Hall effect OUTA: A2 (Pin 4)
					-Hall effect OUTB: A3 (Pin 5)
					
					-Motor 3:
					-Hall effect OUTA: A4 (Pin 6)
					-Hall effect OUTB: A5 (Pin 7)
					
					-Motor 4:
					-Hall effect OUTA: A6 (Pin 14)
					-Hall effect OUTB: A7 (Pin 13) (Yes, these are right)
					
Board Layout (Top View):	1---F---2
							|		|
							3---B---4
*******************************************************************************/

#ifndef MOTORENCODERS_H
#define	MOTORENCODERS_H

#include <stdint.h>

//May need to change based on physical implementation
#define FORWARD 1
#define REVERSE -1

typedef union timerCount {
	uint16_t value;
	char byte[2];
} timerCount;

typedef struct encoderData{
	timerCount pulsePeriod;
	char direction;
} encoderData;

void initializeEncoders(void);
bool pollEncoder(encoderData*);


	

#endif	/* MOTORENCODERS_H */