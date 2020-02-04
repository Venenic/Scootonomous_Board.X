/**************************************************************
Created by: Kyle Hedges
Last Modified: Feb 2, 2020

TODO: Efficiency can be improved by splitting the short long into 3 bytes
***************************************************************/

//Range of 24 bit signed value is -8,388,608 to 8,388,607
//stringOut is 9 long to hold 1 sign, 7 numbers, null character

#include "binaryToString.h"
#include "xc.h"

void convert24Bit(signed long binInput, char* charAddr)
{
	//Check sign 
	if(binInput & 0x800000)
	{
		//Negative
		binInput = ~binInput;
		binInput += 1;
		*charAddr = '-';
	} else *charAddr = '+';
	
	//Conversion consist of shifting the binary number left into the bcd number
	//When any single bcd digit is above 5, 3 is added to it
	
    unsigned long bcdOutput = 0;
    unsigned char i = 0;
	//4 MSB of binary number
	for(i = 0; i < 4; i++)
	{
		if(bcdOutput >= 5) bcdOutput += 3; //Add 3 to ones for conversion
		
		bcdOutput <<= 1;
		//Shift the MSB from binInput into bcdOutput	
		if(binInput & 0x800000) bcdOutput += 1; 
		binInput <<= 1;
	}
	
	//8 MSB of binary number
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput & 0x00000F) >= 0x000005) bcdOutput += 0x000003;	//Add 3 to ones
		if((bcdOutput & 0x0000F0) >= 0x000050) bcdOutput += 0x000030;	//Add 3 to tens
		
		bcdOutput <<= 1;
		if(binInput & 0x800000) bcdOutput += 1; //Shift binary in
		binInput <<= 1;	
	}
	
	//12 MSB of binary number
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput & 0x00000F) >= 0x000005) bcdOutput += 0x000003;	//Add 3 to ones
		if((bcdOutput & 0x0000F0) >= 0x000050) bcdOutput += 0x000030;	//Add 3 to tens
		if((bcdOutput & 0x000F00) >= 0x000500) bcdOutput += 0x000300;	//Add 3 to hundreds
		
		bcdOutput <<= 1;
		if(binInput & 0x800000) bcdOutput += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//16 bits
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput & 0x00000F) >= 0x000005) bcdOutput += 0x000003;	//Add 3 to ones
		if((bcdOutput & 0x0000F0) >= 0x000050) bcdOutput += 0x000030;	//Add 3 to tens
		if((bcdOutput & 0x000F00) >= 0x000500) bcdOutput += 0x000300;	//Add 3 to hundreds
		if((bcdOutput & 0x00F000) >= 0x005000) bcdOutput += 0x003000;	//Add 3 to thousands
		
		bcdOutput <<= 1;
		if(binInput & 0x800000) bcdOutput += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//20 bits
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput & 0x00000F) >= 0x000005) bcdOutput += 0x000003;	//Add 3 to ones
		if((bcdOutput & 0x0000F0) >= 0x000050) bcdOutput += 0x000030;	//Add 3 to tens
		if((bcdOutput & 0x000F00) >= 0x000500) bcdOutput += 0x000300;	//Add 3 to hundreds
		if((bcdOutput & 0x00F000) >= 0x005000) bcdOutput += 0x003000;	//Add 3 to thousands
		if((bcdOutput & 0x0F0000) >= 0x050000) bcdOutput += 0x030000;	//Add 3 to ten thousands
		
		bcdOutput <<= 1;
		if(binInput & 0x800000) bcdOutput += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//All bits
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput & 0x00000F) >= 0x000005) bcdOutput += 0x000003;	//Add 3 to ones
		if((bcdOutput & 0x0000F0) >= 0x000050) bcdOutput += 0x000030;	//Add 3 to tens
		if((bcdOutput & 0x000F00) >= 0x000500) bcdOutput += 0x000300;	//Add 3 to hundreds
		if((bcdOutput & 0x00F000) >= 0x005000) bcdOutput += 0x003000;	//Add 3 to thousands
		if((bcdOutput & 0x0F0000) >= 0x050000) bcdOutput += 0x030000;	//Add 3 to hundred thousands
		if((bcdOutput & 0xF00000) >= 0x500000) bcdOutput += 0x300000;	//Add 3 to millions
		
		bcdOutput <<= 1;
		if(binInput & 0x800000) bcdOutput += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//Backfill the seven digits
	charAddr += 7;
	for(i = 0; i < 7; i++)
	{
		// +/-,_,_,_,_,_,_,_,'\0.
		*charAddr = '0' + ((char) bcdOutput & 0x0F);
		charAddr -= 1;
		bcdOutput >>= 4;
	}
}
