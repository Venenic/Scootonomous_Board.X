/*******************************************************************************
File: 			binaryToString.cabs
Authors:		Kyle Hedges
Created:		Feb. 2, 2020
Last Modified:	Mar. 5, 2020
(c) 2020 Lakehead University

Intended for 8 bit PIC microcontrollers

Description: 	This file contains functions to convert a signed binary number
				to a string of characters representing it in decimal form.		
*******************************************************************************/


//stringOut is 9 long to hold 1 sign, 7 numbers, null character

//Divided 16/24 bits into seperate functions for effeciency
//Si
#include "binaryToString.h"
#include "xc.h"


//so conver24Bit: ---------------------------------------------------
// Parameters:	binInput: The 24 bit binary number to be converted
//				charAddr: Location of resulting string. Should hold 9 characters
//				sign:	  Indicates if binInput is a signed or unsigned value
// Returns:		void
//
// Description:	Converts a signed 24 bit binary number to a 7 digit number.
//				The resulting string is 9 characters long. One sign character,
//				7 numerical digits and a null termination.
// Last Modified:	Feb 19, 2020
//------------------------------------------------------------------------------
void convert24Bit(int32_t binInput, char* charAddr, char sign)
{
	//Range of 24 bit signed value is -8,388,608 to 8,388,607
	unsigned char bcdOutput[4];
	bcdOutput[0] = 0;
	bcdOutput[1] = 0;
	bcdOutput[2] = 0;
	bcdOutput[3] = 0;
	
	//Check sign 
	if((binInput & 0x800000) && (sign == SIGNED))
	{
		//Negative. Take two's compliment
		binInput = ~binInput;
		binInput += 1;
		*charAddr = '-';
	} else *charAddr = '+';
	
	//Conversion consist of shifting the binary number left into the bcd number
	//When any single bcd digit is above 5, 3 is added to it
	
    unsigned char i = 0;
	//4 MSB of binary number. Max number: 16
	for(i = 0; i < 4; i++)
	{
		if(bcdOutput[0] >= 5) bcdOutput[0] += 3; //Add 3 to ones for conversion
		
		bcdOutput[0] <<= 1;
		//Shift the MSB from binInput into bcdOutput	
		if(binInput & 0x800000) bcdOutput[0] += 1; 
		binInput <<= 1;
	}
	
	//8 MSB of binary number. Max number: 2,56
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput[0] & 0x0F) >= 0x05) bcdOutput[0] += 0x03;	//Add 3 to ones
		if((bcdOutput[0] & 0xF0) >= 0x50) bcdOutput[0] += 0x30;	//Add 3 to tens
		
		bcdOutput[1] <<= 1;
		if(bcdOutput[0] & 0x80) bcdOutput[1] += 1;
		bcdOutput[0] <<= 1;
		if(binInput & 0x800000) bcdOutput[0] += 1; //Shift binary in
		binInput <<= 1;	
	}
	
	//12 MSB of binary number. Max number: 40,96
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput[0] & 0x0F) >= 0x05) bcdOutput[0] += 0x03;	//Add 3 to ones
		if((bcdOutput[0] & 0xF0) >= 0x50) bcdOutput[0] += 0x30;	//Add 3 to tens
		if((bcdOutput[1] & 0x0F) >= 0x05) bcdOutput[1] += 0x03;	//Add 3 to hundreds
		
		bcdOutput[1] <<= 1; 
		if(bcdOutput[0] & 0x80) bcdOutput[1] += 1;
		bcdOutput[0] <<= 1;
		if(binInput & 0x800000) bcdOutput[0] += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//16 MSB bits of binary number. Max number: 6,55,36
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput[0] & 0x0F) >= 0x05) bcdOutput[0] += 0x03;	//Add 3 to ones
		if((bcdOutput[0] & 0xF0) >= 0x50) bcdOutput[0] += 0x30;	//Add 3 to tens
		if((bcdOutput[1] & 0x0F) >= 0x05) bcdOutput[1] += 0x03;	//Add 3 to hundreds
		if((bcdOutput[1] & 0xF0) >= 0x50) bcdOutput[1] += 0x30;	//Add 3 to thousands
		
		bcdOutput[2] <<= 1;
		if(bcdOutput[1] & 0x80) bcdOutput[2] += 1;
		bcdOutput[1] <<= 1; 
		if(bcdOutput[0] & 0x80) bcdOutput[1] += 1;
		bcdOutput[0] <<= 1;
		if(binInput & 0x800000) bcdOutput[0] += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//20 MSB bits of binary number. Max number: 1,04,85,76
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput[0] & 0x0F) >= 0x05) bcdOutput[0] += 0x03;	//Add 3 to ones
		if((bcdOutput[0] & 0xF0) >= 0x50) bcdOutput[0] += 0x30;	//Add 3 to tens
		if((bcdOutput[1] & 0x0F) >= 0x05) bcdOutput[1] += 0x03;	//Add 3 to hundreds
		if((bcdOutput[1] & 0xF0) >= 0x50) bcdOutput[1] += 0x30;	//Add 3 to thousands
		if((bcdOutput[2] & 0x0F) >= 0x05) bcdOutput[2] += 0x03;	//Add 3 to ten thousands
		if((bcdOutput[2] & 0xF0) >= 0x50) bcdOutput[2] += 0x30;	//Add 3 to ten thousands
		
		if(bcdOutput[2] & 0x80) bcdOutput[3] += 1;
		bcdOutput[2] <<= 1; 
		if(bcdOutput[1] & 0x80) bcdOutput[2] += 1;
		bcdOutput[1] <<= 1; 
		if(bcdOutput[0] & 0x80) bcdOutput[1] += 1;
		bcdOutput[0] <<= 1;
		if(binInput & 0x800000) bcdOutput[0] += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//24 MSB bits of binary number. Max number: 16,77,72,16
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput[0] & 0x0F) >= 0x05) bcdOutput[0] += 0x03;	//Add 3 to ones
		if((bcdOutput[0] & 0xF0) >= 0x50) bcdOutput[0] += 0x30;	//Add 3 to tens
		if((bcdOutput[1] & 0x0F) >= 0x05) bcdOutput[1] += 0x03;	//Add 3 to hundreds
		if((bcdOutput[1] & 0xF0) >= 0x50) bcdOutput[1] += 0x30;	//Add 3 to thousands
		if((bcdOutput[2] & 0x0F) >= 0x05) bcdOutput[2] += 0x03;	//Add 3 to ten thousands
		if((bcdOutput[2] & 0xF0) >= 0x50) bcdOutput[2] += 0x30;	//Add 3 to hundred thousands
		if((bcdOutput[3] & 0xF0) >= 0x50) bcdOutput[2] += 0x30;	//Add 3 to millions
		
		
		bcdOutput[3] <<= 1;
		if(bcdOutput[2] & 0x80) bcdOutput[3] += 1;
		bcdOutput[2] <<= 1; 
		if(bcdOutput[1] & 0x80) bcdOutput[2] += 1;
		bcdOutput[1] <<= 1; 
		if(bcdOutput[0] & 0x80) bcdOutput[1] += 1;
		bcdOutput[0] <<= 1;
		if(binInput & 0x800000) bcdOutput[0] += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//Backfill the seven digits if signed, or 8 digits if unsigned
	//Yes, this is a lot of copy paste. No, a for loop would not be nicer.
	if(sign = UNSIGNED) charAddr += 8;
    else charAddr += 7;

	// +/-,_,_,_,_,_,_,_,'\0.
	*charAddr = '0' + (bcdOutput[0] & 0x0F);
	charAddr -= 1;
	
	*charAddr = '0' + ((bcdOutput[0] & 0xF0) >> 4);
	charAddr -= 1;
	
	*charAddr = '0' + (bcdOutput[1] & 0x0F);
	charAddr -= 1;
	
	*charAddr = '0' + ((bcdOutput[1] & 0xF0) >> 4);
	charAddr -= 1;
	
	*charAddr = '0' + (bcdOutput[2] & 0x0F);
	charAddr -= 1;
	
	*charAddr = '0' + ((bcdOutput[2] & 0xF0) >> 4);
	charAddr -= 1;
	
	*charAddr = '0' + (bcdOutput[3] & 0x0F);
    
    if(sign == UNSIGNED){
        charAddr -= 1;
		
        *charAddr = '0' + ((bcdOutput[3] & 0xF0) >> 4);
    }
	
}//eo convert24Bit--------------------------------------------------------------


//so conver16Bit: ---------------------------------------------------
// Parameters:	binInput: The 16 bit binary number to be converted
//				charAddr: Location of resulting string. Should hold 9 characters
//				sign:	  Indicates if binInput is a signed or unsigned value
// Returns:		void
//
// Description:	Converts a signed 24 bit binary number to a 7 digit number.
//				The resulting string is 9 characters long. One sign character,
//				7 numerical digits and a null termination.
// Last Modified:	Feb 19, 2020
//------------------------------------------------------------------------------
void convert16Bit(int16_t binInput, char* charAddr, char sign)
{
	//Range of 16 bit signed value is -65,535 to 65,536
	unsigned char bcdOutput[3];
	bcdOutput[0] = 0;
	bcdOutput[1] = 0;
	bcdOutput[2] = 0;

	//Check sign 
	if((binInput & 0x8000) && (sign == SIGNED))
	{
		//Negative. Take two's compliment
		binInput = ~binInput;
		binInput += 1;
		*charAddr = '-';
	} else *charAddr = '+';
	
	//Conversion consist of shifting the binary number left into the bcd number
	//When any single bcd digit is above 5, 3 is added to it
	
    unsigned char i = 0;
	//4 MSB of binary number. Max number: 16
	for(i = 0; i < 4; i++)
	{
		if(bcdOutput[0] >= 5) bcdOutput[0] += 3; //Add 3 to ones for conversion
		
		bcdOutput[0] <<= 1;
		//Shift the MSB from binInput into bcdOutput	
		if(binInput & 0x8000) bcdOutput[0] += 1; 
		binInput <<= 1;
	}
	
	//8 MSB of binary number. Max number: 2,56
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput[0] & 0x0F) >= 0x05) bcdOutput[0] += 0x03;	//Add 3 to ones
		if((bcdOutput[0] & 0xF0) >= 0x50) bcdOutput[0] += 0x30;	//Add 3 to tens
		
		//Shift all the bits left
        bcdOutput[1] <<= 1;
		if(bcdOutput[0] & 0x80) bcdOutput[1] += 1;
		bcdOutput[0] <<= 1;
		if(binInput & 0x8000) bcdOutput[0] += 1; //Shift binary in
		binInput <<= 1;	
	}
	
	//12 MSB of binary number. Max number: 40,96
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput[0] & 0x0F) >= 0x05) bcdOutput[0] += 0x03;	//Add 3 to ones
		if((bcdOutput[0] & 0xF0) >= 0x50) bcdOutput[0] += 0x30;	//Add 3 to tens
		if((bcdOutput[1] & 0x0F) >= 0x05) bcdOutput[1] += 0x03;	//Add 3 to hundreds
		
		bcdOutput[1] <<= 1; 
		if(bcdOutput[0] & 0x80) bcdOutput[1] += 1;
		bcdOutput[0] <<= 1;
		if(binInput & 0x8000) bcdOutput[0] += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//16 MSB of binary number. Max number: 6,55,36
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput[0] & 0x0F) >= 0x05) bcdOutput[0] += 0x03;	//Add 3 to ones
		if((bcdOutput[0] & 0xF0) >= 0x50) bcdOutput[0] += 0x30;	//Add 3 to tens
		if((bcdOutput[1] & 0x0F) >= 0x05) bcdOutput[1] += 0x03;	//Add 3 to hundreds
		if((bcdOutput[1] & 0xF0) >= 0x50) bcdOutput[1] += 0x30;	//Add 3 to thousands
		
        bcdOutput[2] <<= 1;
		if(bcdOutput[1] & 0x80) bcdOutput[2] += 1;
		bcdOutput[1] <<= 1; 
		if(bcdOutput[0] & 0x80) bcdOutput[1] += 1;
		bcdOutput[0] <<= 1;
		if(binInput & 0x8000) bcdOutput[0] += 1; //Shift binary in
		binInput <<= 1;
	}
	
	
	//Backfill the five digits
	charAddr += 5;
	
	// +/-,_,_,_,_,_,'\0.
	*charAddr = '0' + (bcdOutput[0] & 0x0F);
	charAddr -= 1;
	
	*charAddr = '0' + ((bcdOutput[0] & 0xF0) >> 4);
	charAddr -= 1;
	
	*charAddr = '0' + (bcdOutput[1] & 0x0F);
	charAddr -= 1;
	
	*charAddr = '0' + ((bcdOutput[1] & 0xF0) >> 4);
	charAddr -= 1;
	
	*charAddr = '0' + (bcdOutput[2] & 0x0F);
}//eo convert16Bit--------------------------------------------------------------
