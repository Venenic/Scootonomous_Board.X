/*******************************************************************************
File: 			binaryToString.cabs
Authors:		Kyle Hedges

(c) 2020 Lakehead University

Intended for 8 bit PIC microcontrollers

Description: 	This file contains functions to convert a signed binary number
				to a string of characters representing it in decimal form.		
*******************************************************************************/

#ifndef BINARYTOSTRING_H
#define	BINARYTOSTRING_H

#include <stdint.h>

typedef char int24String[9]; //One sign, 5 numbers, null character
typedef char int16String[7]; //One sign, 7 numbers, null character

// conver24Bit: ---------------------------------------------------
// Parameters:	int32_t: The 24 bit binary number to be converted
//				char* : Location of resulting string. Should hold 9 characters
// Returns:		void
//
// Description:	Converts a signed 24 bit binary number to a 7 digit number.
//				The resulting string is 9 characters long. One sign character,
//				7 numerical digits and a null termination.
//------------------------------------------------------------------------------
void convert24Bit(int32_t, char*);


// conver16Bit: ---------------------------------------------------
// Parameters:	binInput: The 16 bit binary number to be converted
//				charAddr: Location of resulting string. Should hold 7 characters
// Returns:		void
//
// Description:	Converts a signed 16 bit binary number to a 5 digit number.
//				The resulting string is 7 characters long. One sign character,
//				5 numerical digits and a null termination.
//------------------------------------------------------------------------------
void convert16Bit(int16_t, char*);

#endif	/* BINARYTOSTRING_H */

