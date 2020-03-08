/*******************************************************************************
File:			motorControl.c
Authors:		Kyle Hedges
Date:			Feb 4, 2020
Last Modified:	Mar 3, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22

Description: 	Controls all four motors
				Duty cycle is 0-200

Hardware Settings:
					-Motor 1 Pins: 	PWM: RD1 (Pin 20)
									Control 1: C0 (Pin 18)
									Control 2: D0 (Pin 19)
									
							Modules: PWM2, P1
							
					-Motor 2 Pins: 	PWM: RC0 (Pin 15)
									Control 1: C2 (Pin 17)
									Control 2: C1 (Pin 16)
									
							Modules: PWM1, P1

					-Motor 3 Pins:  PWM: RC6 (Pin 25)
 *                                  Control 1: D4 (Pin 27)
 *                                  Control 2: C7 (Pin 26)
 * 
                            Modules: PWM 2, P2
 * 
 *                  -Motor 3 Pins:  PWM: RD7 (Pin 30)
 *                                  Control 1: D5 (Pin 29)
 *                                  Control 2: D6 (Pin 28)
 * 
 *                          Modules: PWM 1, P2
 
Board Layout (Top View):	1---F---3
							|		|
							2---B---4
*******************************************************************************/  

#ifndef MOTORCONTROL_H
#define	MOTORCONTROL_H

#include <stdint.h>

//May need to change based on physical implementation
#define M_FORWARD 0
#define M_REVERSE 1
#define M_BRAKE 2
#define M_STOP 3

#define STOP 0
#define FORWARD 1
#define REVERSE 2
#define LEFT 3
#define RIGHT 4
#define BRAKE 5

#define NUMBER_OF_MOTORS 4

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
void updateMotorSpeeds(char, unsigned char, unsigned char, unsigned char, unsigned char);

#endif	/* MOTORCONTROL_H */

