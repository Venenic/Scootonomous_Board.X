/*******************************************************************************
File:			motorControl.h
Authors:		Kyle Hedges
Date:			Feb 4, 2020
Last Modified:	Feb 16, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

Description: 

Board Layout (Top View):	1---F---2
							|		|
							3---B---4

*******************************************************************************/ 

#ifndef MOTORCONTROL_H
#define	MOTORCONTROL_H

#include <stdint.h>

//May need to change based on physical implementation
#define FORWARD 1
#define REVERSE 0

typedef struct motor {
	uint16_t pulseTime;
	unsigned char direction;
	uint16_t dutyCyle;
		
} motor;

//so initializeSysTick: --------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description:	Initializes
//------------------------------------------------------------------------------
void initializeMotorControl(void);

bool pollEncoder(motor *,char);


	

#endif	/* MOTORCONTROL_H */

