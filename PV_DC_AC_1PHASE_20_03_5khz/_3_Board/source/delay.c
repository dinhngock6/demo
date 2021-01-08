/*
 * delay.c
 *
 *  Created on: 26 Nov 2018
 *      Author: Dinh Ngoc
 */

#include"delay.h"

void DelayMs(unsigned long ms)
{
	unsigned long count = 0;
	for(count = 0; count < ms ; count++)
	{
		DelayUs(1000);
	}
}

void DelayS(unsigned long s)
{
	unsigned long count = 0;
	for(count = 0; count < s ; count++)
	{
		DelayMs(1000);
	}
}


