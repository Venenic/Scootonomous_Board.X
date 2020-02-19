/* 
 * File:   binaryToString.h
 * Author: khedg
 *
 * Created on February 19, 2020, 11:00 AM
 */
#include <stdint.h>

typedef char int16String[9]; //One sign, 5 numbers, null character
typedef char int24String[9]; //One sign, 7 numbers, null character

void convert24Bit(int32_t, char*);
void convert16Bit(int16_t, char*);

#ifndef BINARYTOSTRING_H
#define	BINARYTOSTRING_H


#endif	/* BINARYTOSTRING_H */

