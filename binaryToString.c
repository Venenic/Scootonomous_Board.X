/**************************************************************
Created by: Kyle Hedges
Last Modified: Feb 19, 2020

***************************************************************/

//Range of 24 bit signed value is -8,388,608 to 8,388,607
//stringOut is 9 long to hold 1 sign, 7 numbers, null character

//Divided 16/24 bits into seperate functions for effeciency
//Si
#include "binaryToString.h"
#include "xc.h"

void convert24Bit(int32_t binInput, char* charAddr)
{
	typedef union bcd32{
		uint32_t value;
		uint8_t byte[4];
	}bcd32;
	
	bcd32 bcdOutput;
	bcdOutput.value = 0;
	
	//Check sign 
	if(binInput & 0x800000)
	{
		//Negative. Take two's compliment
		binInput = ~binInput;
		binInput += 1;
		*charAddr = '-';
	} else *charAddr = '+';
	
	//Conversion consist of shifting the binary number left into the bcd number
	//When any single bcd digit is above 5, 3 is added to it
	
    unsigned char i = 0;
	//4 MSB of binary number
	for(i = 0; i < 4; i++)
	{
		if(bcdOutput.byte[0] >= 5) bcdOutput.byte[0] += 3; //Add 3 to ones for conversion
		
		bcdOutput.byte[0] <<= 1;
		//Shift the MSB from binInput into bcdOutput	
		if(binInput & 0x800000) bcdOutput.byte[0] += 1; 
		binInput <<= 1;
	}
	
	//8 MSB of binary number
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput.byte[0] & 0x0F) >= 0x05) bcdOutput.byte[0] += 0x03;	//Add 3 to ones
		if((bcdOutput.byte[0] & 0xF0) >= 0x50) bcdOutput.byte[0] += 0x30;	//Add 3 to tens
		
		if(bcdOutput.byte[0] & 0x80) bcdOutput.byte[1] += 1;
		bcdOutput.byte[0] <<= 1;
		if(binInput & 0x800000) bcdOutput.byte[0] += 1; //Shift binary in
		binInput <<= 1;	
	}
	
	//12 MSB of binary number
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput.byte[0] & 0x0F) >= 0x05) bcdOutput.byte[0] += 0x03;	//Add 3 to ones
		if((bcdOutput.byte[0] & 0xF0) >= 0x50) bcdOutput.byte[0] += 0x30;	//Add 3 to tens
		if((bcdOutput.byte[1] & 0x0F) >= 0x05) bcdOutput.byte[1] += 0x03;	//Add 3 to hundreds
		
		bcdOutput.byte[1] <<= 1; 
		if(bcdOutput.byte[0] & 0x80) bcdOutput.byte[1] += 1;
		bcdOutput.byte[0] <<= 1;
		if(binInput & 0x800000) bcdOutput.byte[0] += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//16 bits
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput.byte[0] & 0x0F) >= 0x05) bcdOutput.byte[0] += 0x03;	//Add 3 to ones
		if((bcdOutput.byte[0] & 0xF0) >= 0x50) bcdOutput.byte[0] += 0x30;	//Add 3 to tens
		if((bcdOutput.byte[1] & 0x0F) >= 0x05) bcdOutput.byte[1] += 0x03;	//Add 3 to hundreds
		if((bcdOutput.byte[1] & 0xF0) >= 0x50) bcdOutput.byte[1] += 0x30;	//Add 3 to thousands
		
		if(bcdOutput.byte[1] & 0x80) bcdOutput.byte[2] += 1;
		bcdOutput.byte[1] <<= 1; 
		if(bcdOutput.byte[0] & 0x80) bcdOutput.byte[1] += 1;
		bcdOutput.byte[0] <<= 1;
		if(binInput & 0x800000) bcdOutput.byte[0] += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//20 bits
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput.byte[0] & 0x0F) >= 0x05) bcdOutput.byte[0] += 0x03;	//Add 3 to ones
		if((bcdOutput.byte[0] & 0xF0) >= 0x50) bcdOutput.byte[0] += 0x30;	//Add 3 to tens
		if((bcdOutput.byte[1] & 0x0F) >= 0x05) bcdOutput.byte[1] += 0x03;	//Add 3 to hundreds
		if((bcdOutput.byte[1] & 0xF0) >= 0x50) bcdOutput.byte[1] += 0x30;	//Add 3 to thousands
		if((bcdOutput.byte[2] & 0x0F) >= 0x05) bcdOutput.byte[2] += 0x03;	//Add 3 to ten thousands
		
		bcdOutput.byte[2] <<= 1; 
		if(bcdOutput.byte[1] & 0x80) bcdOutput.byte[2] += 1;
		bcdOutput.byte[1] <<= 1; 
		if(bcdOutput.byte[0] & 0x80) bcdOutput.byte[1] += 1;
		bcdOutput.byte[0] <<= 1;
		if(binInput & 0x800000) bcdOutput.byte[0] += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//All bits
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput.byte[0] & 0x0F) >= 0x05) bcdOutput.byte[0] += 0x03;	//Add 3 to ones
		if((bcdOutput.byte[0] & 0xF0) >= 0x50) bcdOutput.byte[0] += 0x30;	//Add 3 to tens
		if((bcdOutput.byte[1] & 0x0F) >= 0x05) bcdOutput.byte[1] += 0x03;	//Add 3 to hundreds
		if((bcdOutput.byte[1] & 0xF0) >= 0x50) bcdOutput.byte[1] += 0x30;	//Add 3 to thousands
		if((bcdOutput.byte[2] & 0x0F) >= 0x05) bcdOutput.byte[2] += 0x03;	//Add 3 to hundred thousands
		if((bcdOutput.byte[2] & 0xF0) >= 0x50) bcdOutput.byte[2] += 0x30;	//Add 3 to millions
		
		if(bcdOutput.byte[2] & 0x80) bcdOutput.byte[3] += 1;
		bcdOutput.byte[2] <<= 1; 
		if(bcdOutput.byte[1] & 0x80) bcdOutput.byte[2] += 1;
		bcdOutput.byte[1] <<= 1; 
		if(bcdOutput.byte[0] & 0x80) bcdOutput.byte[1] += 1;
		bcdOutput.byte[0] <<= 1;
		if(binInput & 0x800000) bcdOutput.byte[0] += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//Backfill the seven digits
	charAddr += 7;
	
	// +/-,_,_,_,_,_,_,_,'\0.
	*charAddr = '0' + (bcdOutput.byte[0] & 0x0F);
	charAddr -= 1;
	
	*charAddr = '0' + ((bcdOutput.byte[0] & 0xF0) >> 4);
	charAddr -= 1;
	
	*charAddr = '0' + (bcdOutput.byte[1] & 0x0F);
	charAddr -= 1;
	
	*charAddr = '0' + ((bcdOutput.byte[1] & 0xF0) >> 4);
	charAddr -= 1;
	
	*charAddr = '0' + (bcdOutput.byte[2] & 0x0F);
	charAddr -= 1;
	
	*charAddr = '0' + ((bcdOutput.byte[2] & 0xF0) >> 4);
	charAddr -= 1;
	
	*charAddr = '0' + (bcdOutput.byte[3] & 0x0F);
}





void convert16Bit(int16_t binInput, char* charAddr)
{
	//Check sign 
	if(binInput & 0x8000)
	{
		//Negative. Take two's compliment
		binInput = ~binInput;
		binInput += 1;
		*charAddr = '-';
	} else *charAddr = '+';
	
	//Conversion consist of shifting the binary number left into the bcd number
	//When any single bcd digit is above 5, 3 is added to it
	
    uint32_t bcdOutput = 0;
    unsigned char i = 0;
	//4 MSB of binary number
	for(i = 0; i < 4; i++)
	{
		if(bcdOutput >= 5) bcdOutput += 3; //Add 3 to ones for conversion
		
		bcdOutput <<= 1;
		//Shift the MSB from binInput into bcdOutput	
		if(binInput & 0x8000) bcdOutput += 1; 
		binInput <<= 1;
	}
	
	//8 MSB of binary number
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput & 0x000F) >= 0x0005) bcdOutput += 0x0003;	//Add 3 to ones
		if((bcdOutput & 0x00F0) >= 0x0050) bcdOutput += 0x0030;	//Add 3 to tens
		
		bcdOutput <<= 1;
		if(binInput & 0x8000) bcdOutput += 1; //Shift binary in
		binInput <<= 1;	
	}
	
	//12 MSB of binary number
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput & 0x000F) >= 0x0005) bcdOutput += 0x0003;	//Add 3 to ones
		if((bcdOutput & 0x00F0) >= 0x0050) bcdOutput += 0x0030;	//Add 3 to tens
		if((bcdOutput & 0x0F00) >= 0x0500) bcdOutput += 0x0300;	//Add 3 to hundreds
		
		bcdOutput <<= 1;
		if(binInput & 0x8000) bcdOutput += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//16 bits
	for(i = 0; i < 4; i++)
	{
		if((bcdOutput & 0x000F) >= 0x0005) bcdOutput += 0x0003;	//Add 3 to ones
		if((bcdOutput & 0x00F0) >= 0x0050) bcdOutput += 0x0030;	//Add 3 to tens
		if((bcdOutput & 0x0F00) >= 0x0500) bcdOutput += 0x0300;	//Add 3 to hundreds
		if((bcdOutput & 0xF000) >= 0x5000) bcdOutput += 0x3000;	//Add 3 to thousands
		
		bcdOutput <<= 1;
		if(binInput & 0x8000) bcdOutput += 1; //Shift binary in
		binInput <<= 1;
	}
	
	//Backfill the five digits
	charAddr += 5;
	for(i = 0; i < 5; i++)
	{
		// +/-,_,_,_,_,_,'\0.
		*charAddr = '0' + ((char) bcdOutput & 0x0F);
		charAddr -= 1;
		bcdOutput >>= 4;
	}
}
