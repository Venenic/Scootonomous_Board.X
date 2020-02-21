/*******************************************************************************
File:			motorControl.h
Authors:		Kyle Hedges
Date:			Feb 4, 2020
Last Modified:	Feb 20, 2020
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
#define M_FORWARD 0
#define M_REVERSE 1
#define M_BRAKE 2
#define M_STOP 3

typedef struct motor {
	unsigned char dutyCycle;
	char mode;		
} motor;

//so initializeSysTick: --------------------------------------------------------
// Parameters:		void
// Returns:			void 
//
// Description:	Initializes
//------------------------------------------------------------------------------
void initializeMotorControl(void);
void updateMotorSpeed(motor*);

#endif	/* MOTORCONTROL_H */

