/*******************************************************************************
File: 			motorEnocders.cabs
Authors:		Kyle Hedges
Created:		Feb. 20, 2020
Last Modified:	Mar. , 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

Description: 	This file reads a motor's speed and direction using two Hall
				effect sensors in quadrature. The time between rising edges of 
				encoder A determines the speed and the state of encoder B at 
				this rising edge determines the direction.
				
Hardware Settings:	
					-Motor 1:
					-Hall effect OUTA: A7 (Pin 13)
					-Hall effect OUTB: A6 (Pin 14)  (Yes, these are right)
					
					-Motor 2:
					-Hall effect OUTA: A4 (Pin 6)
					-Hall effect OUTB: A5 (Pin 7)
					
					-Motor 3:
					-Hall effect OUTA: B0 (Pin 33)
					-Hall effect OUTB: A1 (Pin 34)
					
					-Motor 4:
					-Hall effect OUTA: B2 (Pin 35)
					-Hall effect OUTB: B3 (Pin 36)
					
Board Layout (Top View):	1---F---3
							|		|
							2---B---4
*******************************************************************************/

#ifndef MOTORENCODERS_H
#define	MOTORENCODERS_H

#include <stdint.h>

//May need to change based on physical implementation
#define ENCODER_REVERSE 0
#define ENCODER_FORWARD 1
#define ENCODER_STOP 2

typedef union timerCount {
	uint16_t value;
	char byte[2];
} timerCount;

typedef int32_t motorSpeed;
typedef int32_t encoderPeriod_t;



void initializeEncoders(void);
bool pollEncoder(motorSpeed*);


	

#endif	/* MOTORENCODERS_H */