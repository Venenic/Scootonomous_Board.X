/*******************************************************************************
File:			hardwareDefinitions.h
Authors:		Kyle Hedges
Date:			Jan 31, 2020
Last Modified:	Feb 3, 2020
(c) 2020 Lakehead University

TARGET DEVICE:PIC18F45K22
Add this header to any file making register accesses.

This file contains defines for various register level hardware values.

*******************************************************************************/
#ifndef HARDWAREDEFINITIONS_H
#define	HARDWAREDEFINITIONS_H

#define INPUT_PIN 1
#define OUTPUT_PIN 0

#define INPUT_PORT 1
#define OUTPUT_PORT 0

#define DIGITAL_INPUT_PIN 0
#define ANALOG_INPUT_PIN 1 

#define LOW_PRIORITY 0
#define HIGH_PRIORITY 1

#define ENABLE_INTERRUPT 1
#define DISABLE_INTERRUPT 0

#define MAX_SLEW_RATE 0
#define LIMITED_SLEW_RATE 1


#endif	/* HARDWAREDEFINITIONS_H */