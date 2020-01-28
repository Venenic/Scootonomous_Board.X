/****************************************************************
File:		initializeHardware.h
Authors:	Kyle Hedges
Date:		Jan 27, 2020
(c) 2020 Lakehead
****************************************************************/

#ifndef INITIALIZEHARDWARE_H
#define	INITIALIZEHARDWARE_H

#define INPUT_PIN 1
#define OUTPUT_PIN 0

void initializeHardware(void);
void initializeADC_CLK(void);
void initializeOscillator(void);

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* INITIALIZEHARDWARE_H */

